#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <format>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <fstream>

#include "flatbuffer/enum_generated.h"
#include "flatbuffer/result_generated.h"

#include "../../libs/cpp/3rdparty/rapidjson/include/rapidjson/rapidjson.h"
#include "../../libs/cpp/3rdparty/rapidjson/include/rapidjson/document.h"
#include "../../libs/cpp/3rdparty/rapidjson/include/rapidjson/writer.h"
#include "../../libs/cpp/3rdparty/rapidjson/include/rapidjson/stringbuffer.h"

// 사칙연산이 없으면 unsigned, 있으면 signed

// idx = 테이블의 인덱스
// id = 서버에서 부여하는 식별자. 서버 재시작시 초기화됨.
// uid = 서버에서 부여하는 식별자. 서버 재시작시 초기화되지 않음.

using Port = uint16_t;

using Time = std::time_t;
using Tick = int64_t;

using TableIdx = int64_t;

using EntityId = uint64_t;
using Percent = int32_t; // 백분율
using Prob = int32_t; // 확률. 0 ~ max_prob 사이의 값으로 표현. 예: 50% = 50000000
constexpr Prob max_prob = 100000000;
using Rate = float; // 증가율. 0.0 ~ 1.0 사이의 값으로 표현. 예: 50% = 0.5

using TerrainIdx = TableIdx;
using TerrainId = uint32_t;
using GridId = uint32_t;

using ChannelId = uint32_t;

using MonsterIdx = TableIdx;
using CharacterIdx = TableIdx;
using GadgetIdx = TableIdx;

using SkillIdx = TableIdx;
using SkillLevel = int32_t;

using ActorId = uint32_t;
using UserUid = uint64_t;

using Level = int32_t;
using Exp = int64_t;

using EffectValue = float;
using Hp = int64_t;
using Mp = int64_t;
using Attack = int64_t;
using Defense = int64_t;

using Damage = Attack;

using ItemUid = uint64_t;
using ItemIdx = TableIdx;
using StackCount = int64_t;
using RefineLevel = int16_t;

using Coord = float;
using Distance = Coord;
using Speed = float;
