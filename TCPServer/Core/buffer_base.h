#pragma once

#include "core_header.h"

namespace core {
enum BufferFlag
{
	None = 0,
	Sequence = 0x01,
	Crc = 0x02,
	Crypt = 0x04,
};

class BufferBase
{
public:
	using Offset_t = std::size_t;
	using Length_t = std::size_t;

	BufferBase() = default;
	virtual ~BufferBase() = default;

	// @brief 버퍼 초기화 - 전체 메모리 초기화
	virtual void Initialize() = 0;
	// @brief 버퍼 초기화 - 메모리 삭제
	virtual void Finalize() = 0;

	// @brief 오프셋만 초기화
	virtual void Clear() = 0;
	// @brief 내부 버퍼 삭제
	virtual void Remove() = 0;
	// @brief 크기 재할당
	virtual bool Resize(Length_t _buffer_length) = 0;
	// @brief 버퍼 메모리 변경
	virtual bool SetBuffer(char* _buffer, Length_t _buffer_length) = 0;

	// @brief 버퍼 크기
	virtual Length_t Length() const = 0;
	// @brief 데이터 크기
	virtual Length_t Size() const = 0;
	// @brief 가용한 크기
	virtual Length_t FreeSize() const = 0;
	// @brief 데이터
	virtual char* Data() const = 0;
	char* operator[](Length_t _offset)
	{
		do
		{
			if (Length() <= _offset)
				break;

			auto data = Data();
			if (nullptr == data)
				break;
			
			return data + _offset;
		} while (false);

		return nullptr;
	}

	// @brief 데이터 읽기
	// @return 읽은 데이터 크기
	virtual Length_t Pop(char* _data, Length_t _length) = 0;
	virtual Length_t Pop(BufferBase* _buffer, Length_t _length) = 0;
	// @brief 데이터 쓰기
	// @reurn 쓴 데이터 크기
	virtual Length_t Push(char* _data, Length_t _length) = 0;

	virtual Offset_t PopOffset() const = 0;
	virtual void SetPopOffset(Offset_t _offset) = 0;
	virtual void AddPopOffset(Offset_t _offset) = 0;

	virtual Offset_t PushOffset() const = 0;
	virtual void SetPushOffset(Offset_t _offset) = 0;
	virtual void AddPushOffset(Offset_t _offset) = 0;

	virtual char* PopPosition() const = 0;
	virtual char* PopPosition(Offset_t _offset) = 0;
	virtual char* PushPosition() const = 0;
	

protected:
	virtual bool Alloc(Length_t _buffer_size) = 0;
	virtual void Destroy() = 0;
};
} // namespace core