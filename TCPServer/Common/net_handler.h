#pragma once

/*
* @brief 네트워크 데이터가 처리될 클래스의 인터페이스 클래스
* @detail SelectEvent, IOCPEvent등의 세부 클래스가 상속함
*/
class NetHandler
{
public:
	NetHandler() = default;
	virtual ~NetHandler() = default;

public:
	virtual bool Initialize() = 0;
	virtual bool Finallize() = 0;
	virtual bool RegistedHandle(HANDLE, ULONG_PTR) = 0;
	virtual bool Start(std::size_t) = 0;
	virtual bool Stop() = 0;
};