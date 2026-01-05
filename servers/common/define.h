#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <future>

#include "flatbuffer/enum_generated.h"
#include "flatbuffer/result_generated.h"

using ActorId = uint32_t;

using Time = std::time_t;

using ItemUid = uint64_t;
using ItemIdx = uint64_t;
using StackCount = int64_t;
using RefineLevel = int16_t;