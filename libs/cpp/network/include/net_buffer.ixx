module;

export module makga.network.buffer;

import makga.lib.buffer;

export namespace makga::network {
export class NetBuffer : public makga::lib::Buffer<char>
{
public:
	NetBuffer();
	NetBuffer(const NetBuffer&) = delete;
	NetBuffer(NetBuffer&&) = delete;
	NetBuffer& operator=(const NetBuffer&) = delete;
	NetBuffer& operator=(NetBuffer&&) = delete;
	virtual ~NetBuffer() = default;
};
} // namespace makga::network