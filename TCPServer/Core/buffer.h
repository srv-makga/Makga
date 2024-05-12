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

	void Initialize() override;
	void Finalize() override;

	void Clear() override;
	void Remove() override;
	bool Resize(Length_t _buffer_size) override;
	bool SetBuffer(char* _buffer, Length_t _buffer_size) override;
	bool Swap(Buffer& _other);

	Length_t Length() const override;
	Length_t Size() const override;
	Length_t FreeSize() const override;
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

private:
	char* m_buffer;
	Length_t m_buffer_length;
	Length_t m_data_size;
};
} // namespace core