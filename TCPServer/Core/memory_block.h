#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

const int k_alignment_default_size = (sizeof(void*));

#define MMGR_POOLINDEX_NULL					(-1)
#define ALIGNMENT_DEFAULT_SIZE				(sizeof(void*))
#define CHUNK_DEFAULT_SIZE					32768
#define GET_OFFSETCOUNT(c,b)				(((c)+(b)-1)/(b))
#define GET_ALIGNMENT_BY_SIZE(c,a)			(((c)+(a)-1)-(((c)+(a)-1)%(a)))
#define CHECK_POWOFTWO(c)					(!((c)&((c)-1)))

#ifdef _DEBUG
#define CHUNK_CHECK_DATA					0xDBDBDBDB
#endif

/*
* @detail block�� ���� ���ϴ� ũ��, chunk�� ����� ũ��
*/
template <int t_alignment = k_alignment_default_size>
class MemoryChunk
{
	typedef unsigned char CHUNK_TYPE;
	typedef CHUNK_TYPE CHUNK_HEAD_TYPE;
#ifdef _DEBUG
	typedef long CHUNK_TAIL_TYPE;
#endif

	enum
	{
		MNGR_CHUNK_ALIGNMENT = t_alignment,
		MNGR_CHUNK_TYPE_SIZE = sizeof(CHUNK_TYPE*),
		MNGR_CHUNK_HEAD_TYPE_SIZE = sizeof(CHUNK_HEAD_TYPE*),
#ifdef _DEBUG
		MNGR_CHUNK_TAIL_TYPE_SIZE = sizeof(CHUNK_TAIL_TYPE),
#endif
	};

public:
	enum CHKMEMORY
	{
		CHKMEMORY_OK,
		CHKMEMORY_NOT_FOUND,
		CHKMEMORY_INVALID_POINTER,
	};

	CHUNK_TYPE* m_available_first_block;		// �Ҵ� ������ �� ������
	CHUNK_HEAD_TYPE* m_available_last_chunk;	// �Ҵ翡 ��� ������ ûũ ������
	int m_gridx;
	int m_block_size;
	int m_chunk_size;
	int m_chunk_head_size;
	int m_alloc_count;							// �Ҵ�� �� ����

	MemoryChunk()
	{
		m_available_first_block = nullptr;
		m_available_last_chunk = nullptr;
		m_gridx = 0;
		m_chunk_size = 0;
		m_block_size = 0;
		m_chunk_head_size = 0;
		m_alloc_count = 0;
	}

	virtual ~MemoryChunk()
	{
		CHUNK_TYPE** free_chunk = reinterpret_cast<CHUNK_TYPE**>(m_available_last_chunk);
		CHUNK_TYPE* next_chunk;

		while (free_chunk)
		{
			next_chunk = *free_chunk;

			::free(free_chunk);

			free_chunk = reinterpret_cast<CHUNK_TYPE**>(next_chunk);
		}
	}

	/*
	* @brief �޸� ûũ �ʱ�ȭ
	* @parameter _block_size : ������ ���(���� ���ϴ�) ũ��
	* @parameter _chunk_size : ������ ûũ(�޸� ���) ũ��
	*/
	bool Initialize(int _block_size, int _chunk_size)
	{
		if (0 >= _block_size)
		{
			return false;
		}

		//
		_block_size = GET_ALIGNMENT_BY_SIZE(_block_size, MNGR_CHUNK_ALIGNMENT);
		//

		int lBlockCount = GET_OFFSETCOUNT(_chunk_size, _block_size);
		if (0 >= lBlockCount)
		{
			return false;
		}

		m_chunk_size = lBlockCount * _block_size;
		m_block_size = _block_size;

		m_chunk_head_size = GET_ALIGNMENT_BY_SIZE(MNGR_CHUNK_HEAD_TYPE_SIZE, MNGR_CHUNK_ALIGNMENT);

		return true;
	}

	void Finalize()
	{

	}

	// @brief chunk �Ҵ�
	void* Allocate()
	{
		if (nullptr == m_available_first_block)
		{
			if (false == AddChunk())
			{
				return nullptr;
			}
		}

		++m_alloc_count;

		CHUNK_TYPE** next_block = reinterpret_cast<CHUNK_TYPE**>(m_available_first_block);
		m_available_first_block = *next_block;

		return reinterpret_cast<void*>(next_block);
	}
	
	// @brief chunk ��ȯ
	void Deallocate(void* _chunk)
	{
		if (nullptr == _chunk)
		{
			return;
		}

		// ���� chunk�� ���� chunk �޸� �ּ� ���� �ְ�
		*(reinterpret_cast<CHUNK_TYPE**>(_chunk)) = m_available_first_block;
		// ���� ��ȯ�Ǵ� chunk�� ��ȯ�ǰ� �����
		m_available_first_block = reinterpret_cast<CHUNK_TYPE*>(pBack);

		--m_alloc_count;
	}

#ifdef _DEBUG
	bool CheckMemory()
	{
		int gridx = m_gridx;

		CHUNK_TYPE** free_chunk = reinterpret_cast<CHUNK_TYPE**>(m_available_last_chunk);
		CHUNK_TYPE* next_chunk;
		CHUNK_TAIL_TYPE* tail;

		while (free_chunk)
		{
			gridx--;

			next_chunk = *free_chunk;

			//
			tail = reinterpret_cast<CHUNK_TAIL_TYPE*>(reinterpret_cast<BYTE*>(free_chunk) + m_chunk_head_size + (m_chunk_size << gridx));
			if (CHUNK_CHECK_DATA != *tail)
			{
				return false;
			}
			//

			free_chunk = reinterpret_cast<CHUNK_TYPE**>(next_chunk);
		}

		if (0 != gridx)
		{
			return false;
		}

		return true;
	}
#endif
	CHKMEMORY CheckMemory(void* pBack)
	{
		long gridx = m_gridx;

		CHUNK_TYPE** free_chunk = reinterpret_cast<CHUNK_TYPE**>(m_available_last_chunk);
		CHUNK_TYPE* next_chunk;
		CHUNK_TYPE* head;
		CHUNK_TYPE* tail;

		while (free_chunk)
		{
			gridx--;

			next_chunk = *free_chunk;

			//
			head = reinterpret_cast<CHUNK_TYPE*>(free_chunk) + m_chunk_head_size;
			tail = reinterpret_cast<CHUNK_TYPE*>(reinterpret_cast<CHUNK_TYPE*>(free_chunk) + m_chunk_head_size + (m_chunk_size << gridx));
			//

			if (reinterpret_cast<CHUNK_TYPE*>(head) <= reinterpret_cast<CHUNK_TYPE*>(pBack) && reinterpret_cast<CHUNK_TYPE*>(tail) > reinterpret_cast<CHUNK_TYPE*>(pBack))
			{
				if (0 != ((reinterpret_cast<CHUNK_TYPE*>(pBack) - reinterpret_cast<CHUNK_TYPE*>(head)) % m_block_size))
				{
					return CHKMEMORY_INVALID_POINTER;
				}

				return CHKMEMORY_OK;
			}
			//

			free_chunk = reinterpret_cast<CHUNK_TYPE**>(next_chunk);
		}

		return CHKMEMORY_NOT_FOUND;
	}

public:
	inline bool IsInitialize() { return (0 != m_chunk_size || 0 != m_block_size); }
	inline long AllocationCount() { return m_alloc_count; }
	inline long BlockSize() { return m_block_size; }

private:
	bool AddChunk()
	{
#ifdef _DEBUG
		long new_alloc_size = m_chunk_head_size + (m_chunk_size << m_gridx) + GET_ALIGNMENT_BY_SIZE(MNGR_CHUNK_TAIL_TYPE_SIZE, MNGR_CHUNK_ALIGNMENT);
#else
		long new_alloc_size = m_chunk_head_size + (m_chunk_size << m_gridx);
#endif

		CHUNK_HEAD_TYPE** new_chunk = reinterpret_cast<CHUNK_HEAD_TYPE**>(malloc(new_alloc_size));
		if (nullptr == new_chunk)
		{
			return false;
		}

		if (nullptr != m_available_last_chunk)
		{
			*new_chunk = m_available_last_chunk;
		}
		else
		{
			*new_chunk = nullptr;
		}

		m_available_last_chunk = reinterpret_cast<CHUNK_HEAD_TYPE*>(new_chunk);
		m_available_first_block = reinterpret_cast<CHUNK_TYPE*>(reinterpret_cast<BYTE*>(m_available_last_chunk) + m_chunk_head_size);
#ifdef _DEBUG
		CHUNK_TYPE* pAvailableLastBlock = reinterpret_cast<CHUNK_TYPE*>(reinterpret_cast<BYTE*>(m_available_last_chunk) + new_alloc_size - m_block_size - GET_ALIGNMENT_BY_SIZE(MNGR_CHUNK_TAIL_TYPE_SIZE, MNGR_CHUNK_ALIGNMENT));
#else
		CHUNK_TYPE* pAvailableLastBlock = reinterpret_cast<CHUNK_TYPE*>(reinterpret_cast<BYTE*>(m_available_last_chunk) + new_alloc_size - m_block_size);
#endif

		//
		CHUNK_TYPE* next_block = m_available_first_block;
		CHUNK_TYPE** ppCurBlock = reinterpret_cast<CHUNK_TYPE**>(m_available_first_block);
		//

		for (; next_block < pAvailableLastBlock;)
		{
			next_block = reinterpret_cast<CHUNK_TYPE*>(reinterpret_cast<BYTE*>(next_block) + m_block_size);
			*ppCurBlock = next_block;
			ppCurBlock = reinterpret_cast<CHUNK_TYPE**>(next_block);
		}

#ifdef _DEBUG
		if (next_block != pAvailableLastBlock)
		{
			return false;
		}

		CHUNK_TAIL_TYPE* tail = reinterpret_cast<CHUNK_TAIL_TYPE*>(reinterpret_cast<BYTE*>(pAvailableLastBlock) + m_block_size);
		*tail = CHUNK_CHECK_DATA;
#endif

		* ppCurBlock = nullptr;
		//

		m_gridx++;

		return true;
	}
};

class MemoryBlockPool
{
public:
	using Chunk = MemoryChunk<>;

public:
	MemoryBlockPool();
	virtual ~MemoryBlockPool();

	void Initialize();

	void* Rental(std::size_t _size);
	void Return(void* _block);

private:
	// @brief �ʱ� Ǯ ����
	void Alloc();
	// @brief ����� ���� ûũ ã��
	Chunk* FindChunk(std::size_t _size);

	std::unordered_map<void*, Chunk*> m_cache;
	std::vector<std::pair<std::size_t, MemoryChunk<>>> m_chunks;
};