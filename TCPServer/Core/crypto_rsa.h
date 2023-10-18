#pragma once

#include "crypto_interface.h"
#include "gmp.h"
#include "Lock.h"

namespace core {
class CryptoRsa : public CryptoInterface
{
public:
	CryptoRsa(const char* p, const char* q);
	virtual ~CryptoRsa();

	bool Encrypt(char* _data, std::size_t _length, std::string& _out) override;
	bool Decrypt(char* _data, std::size_t _length, std::string& _out) override;

protected:
	bool m_key_set;
	mpz_t m_p, m_q, m_n, m_d, m_e;
};
} // namespace core