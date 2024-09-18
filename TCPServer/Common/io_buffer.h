#pragma once

#include "../Core/buffer_ring.hpp"
#include "../Core/lock.h"
#include "../Core/crypto_interface.h"

class IOBuffer
{
public:
	using Buffer_t = core::BufferRing<char>;

public:
	IOBuffer(std::size_t _buffer_size, core::BufferFlag _flag);
	virtual ~IOBuffer();

	bool Push(char* _data, std::size_t _length);
	bool Pop(char* _data, std::size_t _length);
	bool Get(char* _data, std::size_t _length);

	std::size_t FreeSize() const;
	std::size_t Size() const;
	void Clear(std::size_t _position);
	void BufferSize(std::size_t _size);
	void Flag(core::BufferFlag _e);

private:
	Buffer_t m_buffer;
	core::BufferFlag m_flag;
	core::CryptoInterface* m_crypto;

	mutable core::RWMutex m_mutex;
};