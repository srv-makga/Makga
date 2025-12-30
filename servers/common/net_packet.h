#pragma once

import makga.lib.buffer;

class NetPacket : public makga::lib::Buffer<char>
{
public:
	using Id = std::uint16_t;
	using Size = std::uint32_t;
	using Key = std::uint8_t;

	struct Header
	{
		Id      packet_id;
		Size    packet_size;
		Key     packet_key;
	};

	NetPacket() = default;
	virtual ~NetPacket() = default;

	Header* GetPacketHeader() const;
	Id GetPacketId() const;
	Size GetPacketSize() const;
	Key GetPacketKey() const;
};