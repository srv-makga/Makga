#include "pch.h"
#include "send_func.h"

void GridMulticast(Coord_t _x, Coord_t _y, Coord_t _z, flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip, UserUid_t _sender)
{
}

void GridMulticast(const fb::PositionT& _position, flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip, UserUid_t _sender)
{
}

void MulticastAll(flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip)
{
}

void MulticastNation(Nation_t _nationIdx, flatbuffers::FlatBufferBuilder& _builder)
{
}

void MulticastExceptNonNation(flatbuffers::FlatBufferBuilder& _builder)
{
}

void MulticastMap(TerrainIdx_t _terrainIdx, flatbuffers::FlatBufferBuilder& _builder)
{
}
