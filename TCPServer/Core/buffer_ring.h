#pragma once
#include "buffer_base.h"

namespace core {
class BufferRing : public BufferBase
{
public:
	BufferRing(Length_t _length);
	BufferRing(char* _buffer, Length_t _length);
	virtual ~BufferRing();

	// @brief ���� �ʱ�ȭ - ��ü �޸� �ʱ�ȭ
	void Initialize() override;
	// @brief ���� �ʱ�ȭ - �޸� ����
	void Finalize() override;

	// @brief �����¸� �ʱ�ȭ
	void Clear() override;
	// @brief ũ�� ���Ҵ�
	bool Realloc(Length_t _buffer_size) override;
	// @brief ���� �޸� ����
	bool Swap(char* _buffer, Length_t _buffer_size) override;

	// @brief ������ �б�
	// @return ���� ������ ũ��
	BufferBase::Length_t Pop(char* _data, Length_t _length) override;
	BufferBase::Length_t Pop(BufferBase* _buffer, Length_t _length) override;
	// @brief ������ ����
	// @reurn �� ������ ũ��
	BufferBase::Length_t Push(char* _data, Length_t _length) override;

	BufferBase::Offset_t PopOffset() const override;
	void SetPopOffset(Offset_t _offset) override;
	void AddPopOffset(Offset_t _offset) override;

	BufferBase::Offset_t PushOffset() const override;
	void SetPushOffset(Offset_t _offset) override;
	void AddPushOffset(Offset_t _offset) override;

	BufferBase::Length_t BufferSize() const override;
	BufferBase::Length_t UsingSize() const override;
	BufferBase::Length_t FreeSize() const override;

	char* PopPosition() const override;
	char* PopPosition(Offset_t _offset) override;
	char* PushPosition() const override;
	char* Buffer() const override;

protected:
	bool Alloc(Length_t _buffer_size) override;
	void Destroy() override;

	void SetBufferSize(Length_t _length);

private:
	char* m_buffer;
	Length_t m_buffer_size;

	Offset_t m_pop_offset;
	Offset_t m_push_offset;

	bool m_is_inside_created;
};
} // namespace core