#pragma once

// @brief Acceptor Interface
// @details ���ϵ��� ������ ó���ϴ� Ŭ������ �������̽�
class Acceptor
{
public:
	Acceptor() = default;
	virtual ~Acceptor() = default;

	virtual std::size_t GetConnectCount() const = 0;
	virtual std::size_t GetMaxConnectCount() const = 0;
};