#pragma once

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

template<typename T>
T GetJson(const rapidjson::Value& _root, const char* key, T _default = T())
{
	if (false == _root.HasMember(key))
	{
		return _default;
	}

	if constexpr (std::is_same_v<T, bool>)
	{
		return _root[key].IsBool() ? _root[key].GetBool() : _default;
	}
	if constexpr (std::is_same_v<T, int64_t>)
	{
		return _root[key].IsInt64() ? _root[key].GetInt64() : _default;
	}
	if constexpr (std::is_same_v<T, uint64_t>)
	{
		return _root[key].IsUint64() ? _root[key].GetUint64() : _default;
	}
	if constexpr (std::is_unsigned_v<T>)
	{
		return _root[key].IsUint() ? _root[key].GetUint() : _default;
	}
	if constexpr (std::is_integral_v<T>)
	{
		return _root[key].IsInt() ? _root[key].GetInt() : _default;
	}
	if constexpr (std::is_floating_point_v<T>)
	{
		return _root[key].IsFloat() ? _root[key].GetFloat() : _default;
	}
	if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>)
	{
		return _root[key].IsString() ? _root[key].GetString() : _default;
	}

	return _default;
}

#define DECLARE_JSON_KEY(type, key, fname)	public: const type& fname() \
	{ \
		if (m_document.HasMember(#key)) { \
			return m_document[#key].Get<type>(); \
		} \
		return type(); \
	} \
	void Set##fname(const type& _val) \
	{ \
		if (m_document.HasMember(#key)) { \
			m_document[#key] = _val; \
		} \
		else { \
			m_document.AddMember(#key, _val, m_document.GetAllocator()); \
		} \
	}