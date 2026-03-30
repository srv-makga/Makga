module;

#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>

export module makga.network.udp.group;

import makga.lib.lock;

namespace makga::network {
export class UdpGroup
{
public:
    UdpGroup() = default;
    virtual ~UdpGroup() = default;

    void AddMember(const std::string& group_name, const std::string& ip, unsigned short port);
    void RemoveMember(const std::string& group_name, const std::string& ip, unsigned short port);

    struct Member { std::string ip; unsigned short port; };

    std::vector<Member> GetMembers(const std::string& group_name) const;

private:
	mutable lib::SharedMutex mutex_;
    std::unordered_map<std::string, std::vector<Member>> groups_;
};
} // namespace makga::network
