module makga.network.buffer;

namespace makga::network {
NetBuffer::NetBuffer()
	: makga::lib::RingBuffer<char>()
{
}
}// namespace makga::network