#pragma once

/*
* @brief ��Ʈ��ũ �����Ͱ� ó���� Ŭ������ �������̽� Ŭ����
* @detail SelectEvent, IOCPEvent���� ���� Ŭ������ �����
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