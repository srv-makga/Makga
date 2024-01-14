#pragma once

#include "game_header.h"

void GridMulticast(Coord_t _x, Coord_t _y, Coord_t _z, flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip, UserUid_t _sender);
void GridMulticast(const fb::PositionT& _position, flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip, UserUid_t _sender);

void MulticastAll(flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip);				// 모두에게 패킷 전송 (모든 상태)
void MulticastNation(Nation_t _nationIdx, flatbuffers::FlatBufferBuilder& _builder);		// 특정 국가에게만 패킷 전송 (플레이중)
void MulticastExceptNonNation(flatbuffers::FlatBufferBuilder& _builder);			// 비시민 제외하고 패킷 전송 (플레이중)
void MulticastMap(TerrainIdx_t _terrainIdx, flatbuffers::FlatBufferBuilder& _builder);			// 특정 맵 내에 패킷 전송 (플레이중)