#pragma once

// https://github.com/yhirose/cpp-httplib
// #define CPPHTTPLIB_OPENSSL_SUPPORT 
// ssl 기능 추가 OpenSSL 라이브러리 링크 필요
// libssl 및 libcrypto

#include "../Core/httplib.h" // std 보다 우선 include 되어야 한다
#include "common_header.h"
#include "../Core/ip_endpoint.h"
#include <future>

class WebServer
{
public:
	WebServer();
	~WebServer();

	bool Initialize();
	void Finalize();

	bool Start(const std::string& _ip, Port_t _port);

	
	std::shared_ptr<httplib::Server> operator->() const
	{
		return m_server;
	}

private:
	std::shared_ptr<httplib::Server> m_server;
};