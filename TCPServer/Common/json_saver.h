#pragma once

#include "common_header.h"
#include "../Core/RapidJson.h"
#include "../Core/lock.h"

#define DECLARE_MEMBER(type, key, fname)	public: const type& get##fname() \
	{ \
		core::ReadLock(m_mutex);\
		if (m_document.HasMember(#key)) {\
			return m_document[#key].Get<type>(); \
		} \
		return type(); \
	} \
	void set##fname(const type& _val) \
	{ \
		core::WriteLock(m_mutex);\
		if (m_document.HasMember(#key)) {\
			m_document[#key] = _val; \
		} \
		else { \
			m_document.AddMember(#key, _val, m_document.GetAllocator()); \
		} \
		m_is_edit = true; \
	}


class JsonSaver
{
public:
	JsonSaver();
	virtual ~JsonSaver();

	void Initialize(fb::eServerType _server_type);
	void OnUpdate();

	bool Load();
	bool Save();

	// memory -> file
	virtual std::string Serialize() = 0;
	// file -> memory
	virtual bool Deserialize() = 0;

protected:
	rapidjson::Document m_document;
	std::string m_file_name;
	std::atomic<bool> m_is_edit;
	core::RWMutex m_mutex;
};