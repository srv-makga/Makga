#pragma once
#include "buffer_base.h"
#include <type_traits>

namespace core {
template <typename T>
concept ElementType = !std::is_pointer_v<T>;

template <typename T>
class has_initialize {
private:
	template <typename U>
	static auto test(int) -> decltype(std::declval<U>().Initialize(), std::true_type());

	template <typename>
	static std::false_type test(...);

public:
	static constexpr bool value = decltype(test<T>(0))::value;
};

// @brief 기능을 제공하는 껍데기 클래스이며, 실제 데이터는 생성시에 전달해야 한다
template<typename T>
requires ElementType<T>
class BufferRing
{
public:
	using Element_t = T;

public:
	BufferRing(T* _buffer, std::size_t _length)
		: m_buffer(_buffer)
		, m_buffer_size(_length)
		, m_read_offset(0)
		, m_write_offset(0)
	{
	}

	virtual ~BufferRing()
	{
		Finalize();
	}

	bool Initialize()
	{
		if (nullptr == m_buffer)
		{
			return false;
		}

		Clear();
	}

	void Finalize()
	{
		Clear();
	}

	std::size_t Write(T* _buffer, std::size_t _size)
	{
		if (nullptr == _buffer || 0 == _size)
		{
			return 0;
		}

		if (FreeSize() < _size)
		{
			return 0;
		}

		std::size_t remain_length = Size() - WriteOffset();
		if (remain_length > _size)
		{
			::memcpy(WritePosition(), _buffer, _size);
		}
		else
		{
			::memcpy(WritePosition(), _buffer, remain_length);
			::memcpy(m_buffer, _buffer + remain_length, _size - remain_length);
		}

		AddWriteOffset(_size);
		return _size;
	}

	std::size_t Read(T* _buffer, std::size_t _size)
	{
		if (nullptr == _buffer || 0 == _size)
		{
			return 0;
		}

		if (Size() < _size)
		{
			return 0;
		}

		std::size_t remain_length = Length() - ReadOffset();
		if (remain_length > _size)
		{
			::memcpy(_buffer, ReadPosition(), _size);
		}
		else
		{
			::memcpy(_buffer, ReadPosition(), remain_length);
			::memcpy(_buffer + remain_length, m_buffer, _size - remain_length);
		}

		AddReadOffset(_size);
		return _size;
	}

	// @brief 버퍼 당기기
	void ReorganizeBuffer()
	{
		if (m_write_offset >= m_read_offset)
		{
			auto data_size = Size();
			if (m_write_offset == m_read_offset)
			{
				m_write_offset = 0;
				m_read_offset = 0;
			}
			else if (FreeSize() < m_buffer_size / 3)
			{
				::memcpy(&m_buffer[0], &m_buffer[m_read_offset], data_size);
				m_read_offset = 0;
				m_write_offset = data_size;
			}
		}
		else
		{
			auto right_data_size = m_buffer_size - m_read_offset;
			auto left_data_size = m_write_offset;

			::memcpy(&m_buffer[right_data_size], &m_buffer[0], left_data_size);
			::memcpy(&m_buffer[0], &m_buffer[m_read_offset], right_data_size);

			m_read_offset = 0;
			m_write_offset = right_data_size + left_data_size;
		}
	}

	void Clear()
	{
		if (true == IsEmpty())
		{
			return;
		}

		std::size_t size = Size();

		for (std::size_t i = 0; i < size; ++i)
		{
			InitElement(m_buffer[i]);
		}

		m_read_offset = 0;
		m_write_offset = 0;
	}

	void ClearOffset()
	{
		m_read_offset = 0;
		m_write_offset = 0;
	}

	bool IsEmpty() const
	{
		return nullptr == m_buffer || 0 == Size();
	}

	std::size_t Length() const
	{
		return m_buffer_size;
	}

	std::size_t Size() const
	{
		if (m_write_offset >= m_read_offset)
		{
			return m_write_offset - m_read_offset;
		}

		return m_buffer_size - m_read_offset;
	}

	std::size_t FreeSize() const
	{
		if (m_write_offset >= m_read_offset)
		{
			return m_buffer_size - m_write_offset;
		}
		else
		{
			return m_read_offset - m_write_offset;
		}
	}

	std::optional<Element_t*> Data() const
	{
		if (true == IsEmpty())
		{
			return std::nullopt;
		}

		return ReadPosition();
	}

	std::size_t WriteOffset() const
	{
		return m_write_offset;
	}

	std::size_t ReadOffset() const
	{
		return m_read_offset;
	}

	char* GetBufferPtr() const
	{
		return m_buffer;
	}

//protected:
	void AddWriteOffset(std::size_t _offset)
	{
		if (m_write_offset + _offset > m_buffer_size)
		{
			_offset -= (m_buffer_size - m_write_offset);
		}

		m_write_offset = (m_write_offset + _offset) % m_buffer_size;
	}

	bool AddReadOffset(std::size_t _offset)
	{
		if (Size() < _offset)
		{
			return false;
		}

		if (m_read_offset + _offset > m_buffer_size)
		{
			_offset -= (m_buffer_size - m_read_offset);
		}

		m_read_offset = (m_read_offset + _offset) % m_buffer_size;

		return true;
	}

	Element_t* WritePosition() const
	{
		if (nullptr == m_buffer)
		{
			return nullptr;
		}

		return m_buffer + m_write_offset;
	}

	Element_t* ReadPosition() const
	{
		if (nullptr == m_buffer)
		{
			return nullptr;
		}

		return m_buffer + m_read_offset;
	}

	void Remove(std::size_t _size)
	{
		std::size_t loop_count = std::min(Size(), _size);
		if (0 >= loop_count)
		{
			return;
		}

		for (std::size_t i = 0; i < loop_count; ++i)
		{
			InitElement(*ReadPosition());
			AddReadOffset(1);
		}
	}

	inline static void InitElement(T& _element)
	{
		// 초기화 함수가 존재하는 경우엔 초기화 함수 호출
		if constexpr (has_initialize<T>::value)
		{
			_element.Initialize();
		}
		else
		{
			_element = T();
		}
	}

protected:
	T* m_buffer;
	std::size_t m_buffer_size;
	std::size_t m_write_offset;
	std::size_t m_read_offset;
};
} // namespace core