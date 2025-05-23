#pragma once

// https://github.com/yhirose/cpp-httplib
// #define CPPHTTPLIB_OPENSSL_SUPPORT 
// ssl ��� �߰� OpenSSL ���̺귯�� ��ũ �ʿ�
// libssl �� libcrypto

#include "../Core/httplib.h" // std ���� �켱 include �Ǿ�� �Ѵ�
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