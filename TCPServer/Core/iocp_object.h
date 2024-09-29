#pragma once

namespace core {
namespace network {
class IOCPObject : public std::enable_shared_from_this<IOCPObject>
{
public:
	IOCPObject() = default;
	virtual ~IOCPObject() = default;
};
} // namespace network
} // namespace core