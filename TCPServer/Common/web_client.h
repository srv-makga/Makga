#pragma once

// https://github.com/yhirose/cpp-httplib
// #define CPPHTTPLIB_OPENSSL_SUPPORT 
// ssl ��� �߰� OpenSSL ���̺귯�� ��ũ �ʿ�
// libssl �� libcrypto

#include "../Core/httplib.h" // std ���� �켱 include �Ǿ�� �Ѵ�
#include "common_header.h"
#include "../Core/ip_endpoint.h"
#include <future>

class WebClient
{
public:
	using Parameters_t = httplib::Params;
public:
	WebClient() = delete;
	WebClient(const std::string& _ip, Port_t _port);
	WebClient(const core::network::IPEndPoint& _ep);
	~WebClient();

	bool Initialize();
	void Finalize();

	std::future<std::string> GetAsync(const std::string& _path, const Parameters_t& _params = {});

	std::shared_ptr<httplib::Client> operator->()
	{
		return m_client;
	}

private:
	std::shared_ptr<httplib::Client> m_client;
};