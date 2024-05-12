#pragma once

#include "buffer_base.h"

namespace core {
// @brief 일반적인 버퍼
// @detail 사용 후 데이터를 앞당길 수 있음
class BufferPull : public BufferBase
{
public:
	using Offset_t = BufferBase::Offset_t;
	using Length_t = BufferBase::Length_t;

public:
	BufferPull();
	BufferPull(Length_t _length);
	BufferPull(char* _buffer, Length_t _length);
	virtual ~BufferPull();

	void Initialize() override;
	void Finalize() override;

	void Clear() override;
	void Remove() override;
	bool Resize(Length_t _buffer_size) override;
	bool SetBuffer(char* _buffer, Length_t _buffer_size) override;

	Length_t Length() const override;
	Length_t FreeSize() const override;
	Length_t Size() const override;
	char* Data() const override;

	Length_t Pop(char* _data, Length_t _length) override;
	Length_t Pop(BufferBase* _buffer, Length_t _length) override;
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
	void Pull();

private:
	char* m_buffer;
	Length_t m_buffer_size;
	Length_t m_pull_size;

	Offset_t m_pop_offset;
	Offset_t m_push_offset;
};
} // namespace core