#pragma once

#include "../Core/buffer_base.h"
#include "../Core/object_pool.h"
#include "../Core/session.h"

class Packet : public core::ObjectPool<Packet*>
{
public:
	using PacketId_t = int32_t;
	using PacketSize_t = int32_t;

	struct PacketHeader
	{
		PacketId_t id;
		PacketSize_t size;
	};

	using Buffer_t = core::BufferBase;
	using Owner_t = core::network::Session;

public:
	Packet(std::size_t _buffer_size, bool _is_encrypt = false);
	virtual ~Packet();

	void Initialize();
	void Finalize();

	char* Data() const;
	void Clear();

	void Encrypt(const char* _key) {}
	bool Decrypt(const char* _key) { return false; }

	template<typename T>
	Packet& operator<<(T& _value);

	template<typename T>
	Packet& operator>>(T& _value);

public:
	PacketId_t GetId() const;
	void SetId(PacketId_t _id);

	PacketSize_t PacketSize() const;
	PacketSize_t DataSize() const;

	Buffer_t* GetBuffer() const;
	void  SetBuffer(Buffer_t* _buffer);

	Owner_t* GetOwner() const;
	void SetOwner(Owner_t* _owner);

	bool IsEncrypt() const;
	void SetEncryptFlag(bool _value);

	static PacketSize_t HeaderSize();

private:
	PacketHeader* m_header;
	Buffer_t* m_buffer;
	Owner_t* m_owner;
	bool m_is_encrypt;
};

template<typename T>
inline Packet& Packet::operator<<(T& _value)
{
	if (m_buffer->FreeSize() < sizeof(T))
	{
		m_buffer->Push(static_cast<char*>(&_value), sizeof(T));
	}

	return *this;
}

template<typename T>
inline Packet& Packet::operator>>(T& _value)
{
	if (m_buffer->Size() < sizeof(T))
	{
		m_buffer->Pop(static_cast<char*>(&_value), sizeof(T));
	}

	return *this;
}
