#pragma once

#include "../Core/session_interface.h"
#include "../Core/net_buffer.h"
#include "../Core/object_pool.h"

class Packet
{
public:
	using PacketId_t = uint32_t;
	using PacketSize_t = uint32_t;

	struct PacketHeader
	{
		PacketSize_t size = 0;
		PacketId_t id = 0;
		uint8_t encryto_key = 0;
	};

	using Buffer_t = core::network::NetBuffer;
	using Owner_t = std::shared_ptr<core::network::SessionInterface>;

public:
	Packet(bool _is_encrypt = false); // @todo packet의 내 버퍼 크기 지정 필요?
	virtual ~Packet();

	void Initialize();
	void Finalize();

	void SetData(char* _data, std::size_t _size);
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

	std::shared_ptr<Buffer_t> GetBuffer() const;
	void  SetBuffer(std::shared_ptr<Buffer_t> _buffer);

	Owner_t GetOwner() const;
	void SetOwner(Owner_t _owner);

	bool IsEncrypt() const;
	void SetEncryptFlag(bool _value);

public: // static
	inline static PacketSize_t HeaderSize();

	inline static void Push(std::shared_ptr<Packet> _packet)
	{
		s_pool.Push(_packet);
	}

	inline static std::shared_ptr<Packet> Pop()
	{
		return s_pool.Pop();
	}

	inline static void InitPool(std::size_t _max_size, std::size_t _extend_size, std::function<std::shared_ptr<Packet>()> _create_func, std::function<void(std::shared_ptr<Packet>)> _destroy_func)
	{
		s_pool.Initialize(_max_size, _extend_size, _create_func, _destroy_func);
	}

	inline static void ClearPool()
	{
		s_pool.Finalize();
	}

	inline static core::ObjectPool<std::shared_ptr<Packet>> s_pool;

private:
	PacketHeader* m_header;
	std::shared_ptr<Buffer_t> m_buffer;
	Owner_t m_owner;
	bool m_is_encrypt;
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
