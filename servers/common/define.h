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

#include "../../3rdparty/rapidjson/include/rapidjson/rapidjson.h"
#include "../../3rdparty/rapidjson/include/rapidjson/document.h"
#include "../../3rdparty/rapidjson/include/rapidjson/writer.h"
#include "../../3rdparty/rapidjson/include/rapidjson/stringbuffer.h"

// 사칙연산이 없으면 unsigned, 있으면 signed

// idx = 데이터 테이블의 고유한 인덱스
// id = 일회성 고유한 번호. 서버가 재시작되면 다시 넘버링
// uid = 영구적 고유한 번호. 서버가 재시작 되어도 고유함이 유지

// 네트워크
using Port = uint16_t;

// 시간
using Time = std::time_t;
using Tick = int64_t;

// 테이블
using TableIdx = uint64_t;

// 시스템
using Percent = int32_t; // 표기 방식
using Prob = int32_t; // 확률. 0 ~ 100,000,000
constexpr Prob max_prob = 100000000;
using Rate = float; // 수치의 비율

// 컨텐츠
using TerrainIdx = TableIdx;
using TerrainId = uint32_t;
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