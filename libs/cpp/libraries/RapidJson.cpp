#include "../libraries/include/RapidJson.h"

float getJsonValue(const rapidjson::Value& _jsonValue, float _default)
{
	if (_jsonValue.IsInt())
		return (float)_jsonValue.GetInt();
	else if (_jsonValue.IsString())
		return  std::stof(_jsonValue.GetString());
	else if (_jsonValue.IsFloat())
		return  _jsonValue.GetFloat();

	return _default;
}

bool getJsonValue(const rapidjson::Value& _jsonValue, bool _default)
{
	return _jsonValue.IsBool() ? _jsonValue.GetBool() : _default;
}

int8_t getJsonValue(const rapidjson::Value& _jsonValue, int8_t _default)
{
	return _jsonValue.IsInt() ? (int8_t)_jsonValue.GetInt() : _default;
}

uint8_t getJsonValue(const rapidjson::Value& _jsonValue, uint8_t _default)
{
	return _jsonValue.IsUint() ? (uint8_t)_jsonValue.GetUint() : _default;
}

int16_t getJsonValue(const rapidjson::Value& _jsonValue, int16_t _default)
{
	return _jsonValue.IsInt() ? (int16_t)_jsonValue.GetInt() : _default;
}

uint16_t getJsonValue(const rapidjson::Value& _jsonValue, uint16_t _default)
{
	return _jsonValue.IsUint() ? (uint16_t)_jsonValue.GetUint() : _default;
}

int32_t getJsonValue(const rapidjson::Value& _jsonValue, int32_t _default)
{
	return _jsonValue.IsInt() ? _jsonValue.GetInt() : _default;
}

uint32_t getJsonValue(const rapidjson::Value& _jsonValue, uint32_t _default)
{
	return _jsonValue.IsUint() ? _jsonValue.GetUint() : _default;
}

int64_t getJsonValue(const rapidjson::Value& _jsonValue, int64_t _default)
{
	return _jsonValue.IsInt64() ? _jsonValue.GetInt64() : _default;
}

uint64_t getJsonValue(const rapidjson::Value& _jsonValue, uint64_t _default)
{
	return _jsonValue.IsUint64() ? _jsonValue.GetUint64() : _default;
}

std::string getJsonValue(const rapidjson::Value& _jsonValue, std::string _default)
{
	return _jsonValue.IsString() ? std::string(_jsonValue.GetString(), _jsonValue.GetStringLength()) : _default;
}