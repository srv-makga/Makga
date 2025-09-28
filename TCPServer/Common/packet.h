#pragma once

#include "../Core/session_interface.h"
#include "../Core/net_buffer.h"
#include "../Core/object_pool.h"

// @todo 패킷 내 버퍼는 외부에서 만들어 넣어주는 걸로 하자.
// 소켓의 recevice 버퍼의 내용은 복사하지 말고, recv 버퍼의 포인터 주소와 패킷 크기만 넣어서 주면 굳이 복사할 필요가 없다
// 버퍼의 포인터만 등록하는 setBuffer 또는 changeBufffer를 만들고
// 아니면 기존 버퍼에 데이터만 복사할 수 있는 copyData 함수를 만들자

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
	Packet(bool _is_encrypt = false); // @todo packet의 내 버퍼 크기 지정 필요?
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
