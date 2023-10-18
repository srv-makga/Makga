#pragma once

#include "singleton.hpp"

namespace core {
class TypeConvert : public pattern::Singleton<TypeConvert>
{
public:
	TypeConvert() = default;
	~TypeConvert() = default;

	int32_t StringToInt32(const char* _value);
	uint32_t StringToUInt32(const char* _value);
	int64_t StringToInt64(const char* _value);
	uint64_t StringToUInt64(const char* _value);
	float StringToFloat(const char* _value);
	double StringToDouble(const char* _value);
	bool StringToBool(const char* _value);
};
} // namespace core

#define TYPE_CONVERT	core::TypeConvert::Instance()