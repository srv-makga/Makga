module;

#include <string>
#include <optional>
#include "../../3rdparty/rapidjson/include/rapidjson/rapidjson.h"
#include "../../3rdparty/rapidjson/include/rapidjson/document.h"
#include "../../3rdparty/rapidjson/include/rapidjson/writer.h"
#include "../../3rdparty/rapidjson/include/rapidjson/stringbuffer.h"

export module makga.lib.json;

namespace makga::lib {
export class JsonValue
{
public:
	using Iterater = rapidjson::Value::MemberIterator;
	using ConstIterater = rapidjson::Value::ConstMemberIterator;

	JsonValue();
	explicit JsonValue(const rapidjson::Value* value);

	bool IsNull() const;
	bool IsBool() const;
	bool IsInt() const;
	bool IsInt64() const;
	bool IsUint() const;
	bool IsUint64() const;
	bool IsDouble() const;
	bool IsString() const;
	bool IsArray() const;
	bool IsObject() const;

	bool GetBool() const;
	int GetInt() const;
	int64_t GetInt64() const;
	uint32_t GetUint() const;
	uint64_t GetUint64() const;
	float GetFloat() const;
	double GetDouble() const;
	std::string GetString() const;

	template<typename T>
	std::optional<T> TryGet(const char* key) const
	{
		auto iter = value_->FindMember(key);
		if (iter == value_->MemberEnd())
		{
			return std::nullopt;
		}

		if constexpr (std::is_same_v<T, bool>)
		{
			return iter->value.IsBool() ? iter->value.GetBool() : std::nullopt;
		}
		if constexpr (std::is_same_v<T, int64_t>)
		{
			return iter->value.IsInt64() ? iter->value.GetInt64() : std::nullopt;
		}
		if constexpr (std::is_same_v<T, uint64_t>)
		{
			return iter->value.IsUint64() ? iter->value.GetUint64() : std::nullopt;
		}
		if constexpr (std::is_unsigned_v<T>)
		{
			return iter->value.IsUint() ? iter->value.GetUint() : std::nullopt;
		}
		if constexpr (std::is_signed_v<T>)
		{
			return iter->value.IsInt() ? iter->value.GetInt() : std::nullopt;
		}
		if constexpr (std::is_same_v<int32_t, T>)
		{
			return iter->value.IsInt() ? iter->value.GetInt() : std::nullopt;
		}
		if constexpr (std::is_same_v<uint32_t, T>)
		{
			return iter->value.IsUint() ? iter->value.GetUint() : std::nullopt;
		}
		if constexpr (std::is_same_v<float, T>)
		{
			return iter->value.IsFloat() ? iter->value.GetFloat() : std::nullopt;
		}
		if constexpr (std::is_same_v<double, T>)
		{
			return iter->value.IsDouble() ? iter->value.GetDouble() : std::nullopt;
		}
		if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>)
		{
			return iter->value.IsString() ? iter->value.GetString() : std::nullopt;
		}

		return std::nullopt;
	}

	bool HasMember(const char* key) const;
	ConstIterater FindMember(const char* key) const;

	JsonValue operator[](const char* key) const;
	JsonValue operator[](size_t index) const;

	size_t Size() const;

	const rapidjson::Value* GetNative() const { return value_; }

private:
	const rapidjson::Value* value_;
};

// JSON 문서를 래핑하는 클래스
export class JsonDocument
{
public:
	JsonDocument() = default;

	bool Parse(const char* json)
	{
		doc_.Parse(json);
		return !doc_.HasParseError();
	}

	bool Parse(const std::string& json)
	{
		return Parse(json.c_str());
	}

	bool HasParseError() const { return doc_.HasParseError(); }
	size_t GetErrorOffset() const { return doc_.GetErrorOffset(); }

	JsonValue GetRoot() const { return JsonValue(&doc_); }
	
	bool HasMember(const char* key) const { return doc_.IsObject() && doc_.HasMember(key); }
	JsonValue operator[](const char* key) const 
	{ 
		if (doc_.IsObject() && doc_.HasMember(key))
			return JsonValue(&doc_[key]);
		return JsonValue();
	}

	rapidjson::Document& GetNative() { return doc_; }
	const rapidjson::Document& GetNative() const { return doc_; }

private:
	rapidjson::Document doc_;
};

// JSON 값을 문자열로 변환
export std::string JsonToString(const rapidjson::Value& value)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	value.Accept(writer);
	return buffer.GetString();
}

export std::string JsonToString(const JsonDocument& doc)
{
	return JsonToString(doc.GetNative());
}
}