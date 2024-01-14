#pragma once

#include "game_header.h"

void GridMulticast(Coord_t _x, Coord_t _y, Coord_t _z, flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip, UserUid_t _sender);
void GridMulticast(const fb::PositionT& _position, flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip, UserUid_t _sender);

void MulticastAll(flatbuffers::FlatBufferBuilder& _builder, UserUid_t _skip);				// ��ο��� ��Ŷ ���� (��� ����)
void MulticastNation(Nation_t _nationIdx, flatbuffers::FlatBufferBuilder& _builder);		// Ư�� �������Ը� ��Ŷ ���� (�÷�����)
void MulticastExceptNonNation(flatbuffers::FlatBufferBuilder& _builder);			// ��ù� �����ϰ� ��Ŷ ���� (�÷�����)
void MulticastMap(TerrainIdx_t _terrainIdx, flatbuffers::FlatBufferBuilder& _builder);			// Ư�� �� ���� ��Ŷ ���� (�÷�����)