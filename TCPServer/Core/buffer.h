#pragma once

#include "buffer_base.h"

namespace core {
// @brief 일반적인 버퍼
class Buffer : public BufferBase
{
public:
	using Offset_t = BufferBase::Offset_t;
	using Length_t = BufferBase::Length_t;

public:
	Buffer();
	Buffer(Length_t _length);
	Buffer(char* _buffer, Length_t _length);
	virtual ~Buffer();

	// @brief 버퍼 초기화 - 전체 메모리 초기화
	void Initialize() override;
	// @brief 버퍼 초기화 - 메모리 삭제
	void Finalize() override;

	// @brief 오프셋만 초기화
	void Clear() override;
	// @brief 크기 재할당
	bool ReSize(Length_t _buffer_size) override;
	// @brief 버퍼 메모리 변경
	bool Swap(char* _buffer, Length_t _buffer_size) override;
	bool Swap(Buffer& _other);

	// @brief 버퍼 크기
	Length_t Length() const override;
	// @brief 데이터 크기
	Length_t Size() const override;
	// @brief 가용한 크기
	Length_t FreeSize() const override;
	// @brief 데이터
	char* Data() const override;

	// @brief 데이터 읽기
	// @return 읽은 데이터 크기
	Length_t Pop(char* _data, Length_t _length) override;
	Length_t Pop(BufferBase* _buffer, Length_t _length) override;
	// @brief 데이터 쓰기
	// @reurn 쓴 데이터 크기
	Length_t Push(char* _data, Length_t _length) override;

	Offset_t PopOffset() const override;
	void SetPopOffset(Offset_t _offset) override;
	void AddPopOffset(Offset_t _offset) override;

	Offset_t PushOffset() const override;
	void SetPushOffset(Offset_t _offset) override;
	void AddPushOffset(Offset_t _offset) override;

	char* PopPosition() const override;
	char* PopPosition(Offset_t _offset) override;
	char* PushPosition() const override;

protected:
	bool Alloc(Length_t _buffer_size) override;
	void Destroy() override;

private:
	char* m_buffer;
	Length_t m_buffer_length;
	Length_t m_data_size;
};
} // namespace core