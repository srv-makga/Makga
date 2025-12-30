#pragma once

//#include <memory>
#include "net_packet.h"

class Dispatcher
{
public:
	Dispatcher() = default;
	virtual ~Dispatcher() = default;

	virtual bool Dispatch(std::shared_ptr<NetPacket> packet) = 0;
};

template<typename T>
class DispatcherTemplate : public Dispatcher
{
public:
	DispatcherTemplate(std::shared_ptr<T> owner)
		: owner_(owner) {}
	virtual ~DispatcherTemplate() = default;

	bool Dispatch(std::shared_ptr<NetPacket> packet) override
	{
		return Execute(owner_, packet);
	}

	virtual bool Execute(std::shared_ptr<T> owner, std::shared_ptr<NetPacket> packet) = 0;

protected:
	std::shared_ptr<T> owner_;
};