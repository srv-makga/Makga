#pragma once

#include "FlatBuffer/Version_generated.h"

#include "FlatBuffer/ResultType_generated.h"
#include "FlatBuffer/CommonEnum_generated.h"
#include "FlatBuffer/CommonTable_generated.h"

#include "FlatBuffer/ClientToServerPid_generated.h"
#include "FlatBuffer/ClientToServerTable_generated.h"

#include "FlatBuffer/ServerToWorldPid_generated.h"
#include "Flatbuffer/ServerToWorldTable_generated.h"

#include "FlatBuffer/ServerToCommunityPid_generated.h"
#include "Flatbuffer/ServerToCommunityTable_generated.h"

#include "FlatBuffer/ServerToDBAgentPid_generated.h"
#include "Flatbuffer/ServerToDBAgentTable_generated.h"

#include "FlatBuffer/ServerToGlobalPid_generated.h"
#include "Flatbuffer/ServerToGlobalTable_generated.h"

using namespace fb;


template<typename T>
static const T* BufferToStruct(const char* _buffer, DWORD _len)
{
	flatbuffers::Verifier verifier((uint8_t*)_buffer, _len);
	if (_len != 0 && !verifier.VerifyBuffer<T>())
	{
		return nullptr;
	}

	auto root = flatbuffers::GetRoot<T>(_buffer);
	if (nullptr == root)
	{
		return nullptr;
	}

	if (false == root->Verify(verifier))
	{
		return nullptr;
	}

	return root;
}

#define PACKET_TO_FBSTRUCT(packet, type)  BufferToStruct<type>(packet->Buffer()->Buffer() + Packet::HeaderSize(), packet->Buffer()->UsingSize() - Packet::HeaderSize());\
	if (nullptr == recv_data) { LOG_ERROR << "Fail to convert packet -> " << #type; return false; }

#define DECLARE_FB_RECEIVE(fb_struct) \
    bool On##fb_struct(Packet::PacketId_t _protocol, const char* _buffer, DWORD _len, Packet* _packet)


#define IMPLEMENT_FB_RECEIVE_BEGIN(fb_namespace, className, fb_struct) \
	bool className::On##fb_struct(Packet::PacketId_t _protocol, const char* _buffer, DWORD _len, Packet* _packet) \
	{ \
		auto recv_data = BufferToStruct<##fb_namespace::##fb_struct>(_buffer, _len);\
		if (_len !=0 && recv_data == nullptr)	\
		{	\
			LOG_ERROR << "flatbuffer Verify() fail - protocolId:" << #fb_struct;\
			return false;	\
		}

#define IMPLEMENT_FB_RECEIVE_END	return true; }