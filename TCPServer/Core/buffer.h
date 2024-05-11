#pragma once

#include "buffer_base.h"

namespace core {
// @brief �Ϲ����� ����
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

	// @brief ���� �ʱ�ȭ - ��ü �޸� �ʱ�ȭ
	void Initialize() override;
	// @brief ���� �ʱ�ȭ - �޸� ����
	void Finalize() override;

	// @brief �����¸� �ʱ�ȭ
	void Clear() override;
	// @brief ũ�� ���Ҵ�
	bool ReSize(Length_t _buffer_size) override;
	// @brief ���� �޸� ����
	bool Swap(char* _buffer, Length_t _buffer_size) override;
	bool Swap(Buffer& _other);

	// @brief ���� ũ��
	Length_t Length() const override;
	// @brief ������ ũ��
	Length_t Size() const override;
	// @brief ������ ũ��
	Length_t FreeSize() const override;
	// @brief ������
	char* Data() const override;

	// @brief ������ �б�
	// @return ���� ������ ũ��
	Length_t Pop(char* _data, Length_t _length) override;
	Length_t Pop(BufferBase* _buffer, Length_t _length) override;
	// @brief ������ ����
	// @reurn �� ������ ũ��
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