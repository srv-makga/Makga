#pragma once

#include "buffer_base.h"

namespace core {
class BufferPull : public BufferBase
{
public:
	using Offset_t = BufferBase::Offset_t;
	using Length_t = BufferBase::Length_t;

public:
	BufferPull() = delete;
	BufferPull(Length_t _length);
	BufferPull(char* _buffer, Length_t _length);
	virtual ~BufferPull();

	// @brief 버퍼 초기화 - 전체 메모리 초기화
	void Initialize() override;
	// @brief 버퍼 초기화 - 메모리 삭제
	void Finalize() override;

	// @brief 오프셋만 초기화
	void Clear() override;
	// @brief 크기 재할당
	bool Realloc(Length_t _buffer_size) override;
	// @brief 버퍼 메모리 변경
	bool Swap(char* _buffer, Length_t _buffer_size) override;

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

	Length_t BufferSize() const override;
	Length_t FreeSize() const override;
	Length_t UsingSize() const override;

	char* PopPosition() const override;
	char* PopPosition(Offset_t _offset) override;
	char* PushPosition() const override;
	char* Buffer() const override;

protected:
	bool Alloc(Length_t _buffer_size) override;
	void Destroy() override;
	void Pull();

private:
	void BufferSize(Length_t _length);

private:
	char* m_buffer;
	Length_t m_buffer_size;
	Length_t m_pull_size;

	Offset_t m_pop_offset;
	Offset_t m_push_offset;

	bool m_is_inside_created;
};
} // namespace core