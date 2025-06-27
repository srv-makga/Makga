#pragma once

// @brief Acceptor Interface
// @details 소켓들의 접속을 처리하는 클래스의 인터페이스
class Acceptor
{
public:
	Acceptor() = default;
	virtual ~Acceptor() = default;

	virtual std::size_t GetConnectCount() const = 0;
	virtual std::size_t GetMaxConnectCount() const = 0;
};