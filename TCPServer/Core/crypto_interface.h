#pragma once

#include "core_header.h"

namespace core {
class CryptoInterface
{
public:
	CryptoInterface() = default;
	virtual ~CryptoInterface() = default;

	virtual void Initialize() = 0;
	virtual bool Encrypt(char* _data, std::size_t _length, std::string& _out) = 0;
	virtual bool Decrypt(char* _data, std::size_t _length, std::string& _out) = 0;
};
} // namespace core