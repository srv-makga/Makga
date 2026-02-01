module;

#include <string>
#include "../../3rdparty/rapidjson/include/rapidjson/rapidjson.h"
#include "../../3rdparty/rapidjson/include/rapidjson/document.h"
#include "../../3rdparty/rapidjson/include/rapidjson/writer.h"
#include "../../3rdparty/rapidjson/include/rapidjson/stringbuffer.h"

export module makga.lib.json;

namespace makga::lib {
export std::string Test()
{
	return "makga.lib.json";
}

template<typename T>
T TryGetJson(const rapidjson::Value& root, const char* key, T default_value = T())
{
	if (false == root.HasMember(key))
	{
		return default_value;
	}

	if constexpr (std::is_same_v<T, bool>)
	{
		return root[key].IsBool() ? root[key].GetBool() : default_value;
	}
	if constexpr (std::is_same_v<T, int64_t>)
	{
		return root[key].IsInt64() ? root[key].GetInt64() : default_value;
	}
	if constexpr (std::is_same_v<T, uint64_t>)
	{
		return root[key].IsUint64() ? root[key].GetUint64() : default_value;
	}
	if constexpr (std::is_unsigned_v<T>)
	{
		return root[key].IsUint() ? root[key].GetUint() : default_value;
	}
	if constexpr (std::is_integral_v<T>)
	{
		return root[key].IsInt() ? root[key].GetInt() : default_value;
	}
	if constexpr (std::is_floating_point_v<T>)
	{
		return root[key].IsFloat() ? root[key].GetFloat() : default_value;
	}
	if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>)
	{
		return root[key].IsString() ? root[key].GetString() : default_value;
	}

	return default_value;
}
}