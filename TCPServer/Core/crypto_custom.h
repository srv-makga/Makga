#pragma once

#include "crypto_interface.h"

namespace core {
template<typename T = int>
class CryptoCustom : public CryptoInterface
{
	T m_key;

public:
	CryptoCustom(T _key)
		: m_key(_key)
	{
	}

	virtual ~CryptoCustom()
	{
	}

	bool Encrypt(char* _data, std::size_t _length, std::string& _out) override
	{
		if (nullptr == _data || 0 == _length)
		{
			return false;
		}

		return false;
	}

	bool Decrypt(char* _data, std::size_t _length, std::string& _out) override
	{
		return false;
	}
};
} // namespace core