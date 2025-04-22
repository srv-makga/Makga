#pragma once

// https://github.com/yhirose/cpp-httplib
// #define CPPHTTPLIB_OPENSSL_SUPPORT 
// ssl 기능 추가 OpenSSL 라이브러리 링크 필요
// libssl 및 libcrypto

#include "../Core/httplib.h" // std 보다 우선 include 되어야 한다
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