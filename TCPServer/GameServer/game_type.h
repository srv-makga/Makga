#pragma once

#include "../Common/common_type.h"
#include <unordered_set>

class Actor;
class User;

using ActorList = std::unordered_set<std::shared_ptr<Actor>>;
using UserList = std::unordered_set<User*>;
using ActorSearchFilter = uint32_t;