#pragma once

#include "../Core/session_interface.h"
#include "../Core/net_buffer.h"
#include "../Core/object_pool.h"

// @todo ��Ŷ �� ���۴� �ܺο��� ����� �־��ִ� �ɷ� ����.
// ������ recevice ������ ������ �������� ����, recv ������ ������ �ּҿ� ��Ŷ ũ�⸸ �־ �ָ� ���� ������ �ʿ䰡 ����
// ������ �����͸� ����ϴ� setBuffer �Ǵ� changeBufffer�� �����
// �ƴϸ� ���� ���ۿ� �����͸� ������ �� �ִ� copyData �Լ��� ������

class Packet
{
public:
	using PacketId_t = uint32_t;
	using PacketSize_t = uint32_t;
	using EncryptKet_t = uint8_t;

	struct PacketHeader
	{
		PacketSize_t size = 0;
		PacketId_t id = 0;
		EncryptKet_t encrypt_key = 0;
	};

	using Buffer_t = core::network::NetBuffer;
	using Owner_t = std::shared_ptr<core::network::SessionInterface>;

public:
	Packet(bool _is_encrypt = false); // @todo packet�� �� ���� ũ�� ���� �ʿ�?
	virtual ~Packet();

	bool Initialize();
	void Finalize();

	void SetData(char* _data, std::size_t _size);
	char* Data() const;

	void Clear();

	void Encrypt(const char* _key);
	bool Decrypt(const char* _key);

	template<typename T>
	Packet& operator<<(T& _value);

	template<typename T>
	Packet& operator>>(T& _value);

public:
	PacketId_t GetId() const;
	void SetId(PacketId_t _id);

	PacketSize_t PacketSize() const;
	PacketSize_t DataSize() const;

	std::shared_ptr<Buffer_t> GetBuffer() const;
	void  SetBuffer(std::shared_ptr<Buffer_t> _buffer);

	Owner_t GetOwner() const;
	void SetOwner(Owner_t _owner);

	bool IsEncrypt() const;
	void SetEncryptKey(EncryptKet_t _key);

public: // static
	inline static PacketSize_t HeaderSize() { return sizeof(PacketHeader); }

	inline static std::shared_ptr<Packet> Pop()
	{
		return s_pool.Pop();
	}

	inline static void InitPool(std::size_t _initial_size, std::function<Packet*()> _create_func)
	{
		s_pool.Initialize(_initial_size, _create_func);
	}

	inline static void ClearPool()
	{
		s_pool.Finalize();
	}

	inline static core::SharedObjectPool<Packet> s_pool;

private:
	PacketHeader* m_header;
	std::shared_ptr<Buffer_t> m_buffer;
	Owner_t m_owner;
};

template<typename T>
inline Packet& Packet::operator<<(T& _value)
{
	if (m_buffer->FreeSize() < sizeof(T))
	{
		m_buffer->Write(static_cast<char*>(&_value), sizeof(T));
	}

	return *this;
}

template<typename T>
inline Packet& Packet::operator>>(T& _value)
{
	if (m_buffer->Size() < sizeof(T))
	{
		m_buffer->Read(static_cast<char*>(&_value), sizeof(T));
	}

	return *this;
}
