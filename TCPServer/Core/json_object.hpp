#pragma once

#include "RapidJson.h"
#include <variant>

class JsonObject
{
public:
	JsonObject()
	{
	}

	~JsonObject()
	{
	}

	void Initialize()
	{
		m_document.Clear();
	}

	bool InitJson(const std::string& _json)
	{
		return !m_document.Parse(_json.c_str()).HasParseError();
	}

	template<typename T>
	T Get(const char* _name) const
	{
		const rapidjson::Value& value = Find(_name, m_document);

		if (true == value.ObjectEmpty())
		{
			return T();
		}

		return value.Get<T>();
	}

protected:
	const rapidjson::Value& Find(const char* _name, rapidjson::Value& _value)
	{
		for (auto iter = _value.MemberBegin(); _value.MemberEnd() != iter; ++iter)
		{
			if (0 == strcmp(iter->name.GetString(), _name))
			{
				return iter->value;
			}

			if (true == iter->value.IsObject())
			{
				return Find(_name, iter->value);
			}

			if (true == iter->value.IsArray())
			{
				for (auto arriter = iter->value.Begin(); iter->value.End() != arriter; ++arriter)
				{
					return Find(_name , *arriter);
				}
			}

			return s_empty_value;
		}
	}

protected:
	rapidjson::Document m_document;
	inline static const rapidjson::Value s_empty_value;
};