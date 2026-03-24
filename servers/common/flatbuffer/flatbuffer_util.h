#pragma once

#include "enum_generated.h"
#include "result_generated.h"
#include "version_generated.h"

#include <string>
#include <unordered_map>
#include <mutex>

// @brief FlatBuffers enum -> EnumNames 함수 매핑 트레이츠
// 새로운 enum 타입 사용 시 특수화(specialization)를 추가한다.
//template<> struct FlatbufferEnumNames<makga::ServerType> {
//	static const char* const* Get() { return makga::EnumNamesServerType(); }
//};
//template<> struct FlatbufferEnumNames<makga::DatabaseType> {
//	static const char* const* Get() { return makga::EnumNamesDatabaseType(); }
//};
//template<> struct FlatbufferEnumNames<makga::RedisType> {
//	static const char* const* Get() { return makga::EnumNamesRedisType(); }
//};
//template<> struct FlatbufferEnumNames<makga::AccountType> {
//	static const char* const* Get() { return makga::EnumNamesAccountType(); }
//};
//template<> struct FlatbufferEnumNames<makga::SessionType> {
//	static const char* const* Get() { return makga::EnumNamesSessionType(); }
//};
//template<> struct FlatbufferEnumNames<makga::MapType> {
//	static const char* const* Get() { return makga::EnumNamesMapType(); }
//};
//template<> struct FlatbufferEnumNames<makga::AIType> {
//	static const char* const* Get() { return makga::EnumNamesAIType(); }
//};
//template<> struct FlatbufferEnumNames<makga::RaceType> {
//	static const char* const* Get() { return makga::EnumNamesRaceType(); }
//};
//template<> struct FlatbufferEnumNames<makga::MonsterFaction> {
//	static const char* const* Get() { return makga::EnumNamesMonsterFaction(); }
//};
//template<> struct FlatbufferEnumNames<makga::SkillType> {
//	static const char* const* Get() { return makga::EnumNamesSkillType(); }
//};
//template<> struct FlatbufferEnumNames<makga::ZoneAttributeType> {
//	static const char* const* Get() { return makga::EnumNamesZoneAttributeType(); }
//};
//
//template<typename T>
//struct FlatbufferEnumNames;
//
//// @brief FlatBuffers 생성 enum 문자열을 enum 값으로 변환
//template<typename T>
//bool StringToEnum(const std::string& str, T& enum_value)
//{
//	static std::unordered_map<std::string, T> s_table;
//	static std::once_flag s_init_flag;
//
//	std::call_once(s_init_flag, []()
//		{
//			const char* const* names = FlatbufferEnumNames<T>::Get();
//			for (size_t i = 0; names[i] != nullptr; ++i)
//			{
//				s_table.emplace(names[i], static_cast<T>(i));
//			}
//		}
//	);
//
//	auto it = s_table.find(str);
//	if (it != s_table.end())
//	{
//		enum_value = it->second;
//		return true;
//	}
//
//	return false;
//}