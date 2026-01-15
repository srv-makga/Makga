#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <future>

#include "flatbuffer/enum_generated.h"
#include "flatbuffer/result_generated.h"

// idx = 데이터 테이블의 고유한 인덱스
// id = 일회성 고유한 번호. 서버가 재시작되면 다시 넘버링
// uid = 영구적 고유한 번호. 서버가 재시작 되어도 고유함이 유지

using Time = std::time_t;
using Tick = unsigned __int64;

using TableIdx = int64_t;

using ChannelId = int32_t;

using TerrainId = uint32_t;
using TerrainIdx = TableIdx;

using Coord = float;

using ActorId = uint32_t;
using UserUid = uint64_t;

using Hp = int64_t;
using Mp = int64_t;

using ItemUid = uint64_t;
using ItemIdx = uint64_t;
using StackCount = int64_t;
using RefineLevel = int16_t;