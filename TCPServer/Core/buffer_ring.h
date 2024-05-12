#pragma once
#include "buffer_base.h"

namespace core {
class BufferRing : public BufferBase
{
public:
	BufferRing(Length_t _length);
	BufferRing(char* _buffer, Length_t _length);
	virtual ~BufferRing();

	void Initialize() override;
	void Finalize() override;

	void Clear() override;
	void Remove() override;
	bool Resize(Length_t _buffer_size) override;
	bool SetBuffer(char* _buffer, Length_t _buffer_size) override;

	BufferBase::Length_t Length() const override;
	BufferBase::Length_t Size() const override;
	BufferBase::Length_t FreeSize() const override;
	char* Data() const override;

	BufferBase::Length_t Pop(char* _data, Length_t _length) override;
	BufferBase::Length_t Pop(BufferBase* _buffer, Length_t _length) override;
	BufferBase::Length_t Push(char* _data, Length_t _length) override;

	BufferBase::Offset_t PopOffset() const override;
	void SetPopOffset(Offset_t _offset) override;
	void AddPopOffset(Offset_t _offset) override;

	BufferBase::Offset_t PushOffset() const override;
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
	Length_t m_buffer_size;

	Offset_t m_pop_offset;
	Offset_t m_push_offset;
};
} // namespace core