// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_COMMONENUM_FB_H_
#define FLATBUFFERS_GENERATED_COMMONENUM_FB_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
              FLATBUFFERS_VERSION_MINOR == 5 &&
              FLATBUFFERS_VERSION_REVISION == 26,
             "Non-compatible flatbuffers version included");

namespace fb {

enum eServerType : int32_t {
  eServerType_None = -3,
  eServerType_All = -2,
  eServerType_User = -1,
  eServerType_Global = 0,
  eServerType_Login = 1,
  eServerType_DBAgent = 2,
  eServerType_World = 3,
  eServerType_Community = 4,
  eServerType_InstancePool = 5,
  eServerType_Instance = 6,
  eServerType_Game = 7,
  eServerType_Web = 8,
  eServerType_MIN = eServerType_None,
  eServerType_MAX = eServerType_Web
};

inline const eServerType (&EnumValueseServerType())[12] {
  static const eServerType values[] = {
    eServerType_None,
    eServerType_All,
    eServerType_User,
    eServerType_Global,
    eServerType_Login,
    eServerType_DBAgent,
    eServerType_World,
    eServerType_Community,
    eServerType_InstancePool,
    eServerType_Instance,
    eServerType_Game,
    eServerType_Web
  };
  return values;
}

inline const char * const *EnumNameseServerType() {
  static const char * const names[13] = {
    "None",
    "All",
    "User",
    "Global",
    "Login",
    "DBAgent",
    "World",
    "Community",
    "InstancePool",
    "Instance",
    "Game",
    "Web",
    nullptr
  };
  return names;
}

inline const char *EnumNameeServerType(eServerType e) {
  if (::flatbuffers::IsOutRange(e, eServerType_None, eServerType_Web)) return "";
  const size_t index = static_cast<size_t>(e) - static_cast<size_t>(eServerType_None);
  return EnumNameseServerType()[index];
}

enum eDBType : int32_t {
  eDBType_Account = 0,
  eDBType_Game = 1,
  eDBType_Mail = 2,
  eDBType_Ranking = 3,
  eDBType_MIN = eDBType_Account,
  eDBType_MAX = eDBType_Ranking
};

inline const eDBType (&EnumValueseDBType())[4] {
  static const eDBType values[] = {
    eDBType_Account,
    eDBType_Game,
    eDBType_Mail,
    eDBType_Ranking
  };
  return values;
}

inline const char * const *EnumNameseDBType() {
  static const char * const names[5] = {
    "Account",
    "Game",
    "Mail",
    "Ranking",
    nullptr
  };
  return names;
}

inline const char *EnumNameeDBType(eDBType e) {
  if (::flatbuffers::IsOutRange(e, eDBType_Account, eDBType_Ranking)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseDBType()[index];
}

enum eRedisType : int32_t {
  eRedisType_Cache = 0,
  eRedisType_Ranking = 1,
  eRedisType_MIN = eRedisType_Cache,
  eRedisType_MAX = eRedisType_Ranking
};

inline const eRedisType (&EnumValueseRedisType())[2] {
  static const eRedisType values[] = {
    eRedisType_Cache,
    eRedisType_Ranking
  };
  return values;
}

inline const char * const *EnumNameseRedisType() {
  static const char * const names[3] = {
    "Cache",
    "Ranking",
    nullptr
  };
  return names;
}

inline const char *EnumNameeRedisType(eRedisType e) {
  if (::flatbuffers::IsOutRange(e, eRedisType_Cache, eRedisType_Ranking)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseRedisType()[index];
}

enum eActorType : int32_t {
  eActorType_None = -1,
  eActorType_Character = 0,
  eActorType_Monster = 1,
  eActorType_Object = 2,
  eActorType_Gadget = 3,
  eActorType_Npc = 4,
  eActorType_Pran = 5,
  eActorType_MIN = eActorType_None,
  eActorType_MAX = eActorType_Pran
};

inline const eActorType (&EnumValueseActorType())[7] {
  static const eActorType values[] = {
    eActorType_None,
    eActorType_Character,
    eActorType_Monster,
    eActorType_Object,
    eActorType_Gadget,
    eActorType_Npc,
    eActorType_Pran
  };
  return values;
}

inline const char * const *EnumNameseActorType() {
  static const char * const names[8] = {
    "None",
    "Character",
    "Monster",
    "Object",
    "Gadget",
    "Npc",
    "Pran",
    nullptr
  };
  return names;
}

inline const char *EnumNameeActorType(eActorType e) {
  if (::flatbuffers::IsOutRange(e, eActorType_None, eActorType_Pran)) return "";
  const size_t index = static_cast<size_t>(e) - static_cast<size_t>(eActorType_None);
  return EnumNameseActorType()[index];
}

enum eMoveType : int32_t {
  eMoveType_None = -1,
  eMoveType_Move = 0,
  eMoveType_TelePort = 1,
  eMoveType_MIN = eMoveType_None,
  eMoveType_MAX = eMoveType_TelePort
};

inline const eMoveType (&EnumValueseMoveType())[3] {
  static const eMoveType values[] = {
    eMoveType_None,
    eMoveType_Move,
    eMoveType_TelePort
  };
  return values;
}

inline const char * const *EnumNameseMoveType() {
  static const char * const names[4] = {
    "None",
    "Move",
    "TelePort",
    nullptr
  };
  return names;
}

inline const char *EnumNameeMoveType(eMoveType e) {
  if (::flatbuffers::IsOutRange(e, eMoveType_None, eMoveType_TelePort)) return "";
  const size_t index = static_cast<size_t>(e) - static_cast<size_t>(eMoveType_None);
  return EnumNameseMoveType()[index];
}

enum eInvenType : int32_t {
  eInvenType_UserItem = 0,
  eInvenType_MIN = eInvenType_UserItem,
  eInvenType_MAX = eInvenType_UserItem
};

inline const eInvenType (&EnumValueseInvenType())[1] {
  static const eInvenType values[] = {
    eInvenType_UserItem
  };
  return values;
}

inline const char * const *EnumNameseInvenType() {
  static const char * const names[2] = {
    "UserItem",
    nullptr
  };
  return names;
}

inline const char *EnumNameeInvenType(eInvenType e) {
  if (::flatbuffers::IsOutRange(e, eInvenType_UserItem, eInvenType_UserItem)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseInvenType()[index];
}

enum eItemType : int32_t {
  eItemType_None = 0,
  eItemType_Equip = 1,
  eItemType_Currency = 2,
  eItemType_Consum = 3,
  eItemType_Invisible = 4,
  eItemType_MIN = eItemType_None,
  eItemType_MAX = eItemType_Invisible
};

inline const eItemType (&EnumValueseItemType())[5] {
  static const eItemType values[] = {
    eItemType_None,
    eItemType_Equip,
    eItemType_Currency,
    eItemType_Consum,
    eItemType_Invisible
  };
  return values;
}

inline const char * const *EnumNameseItemType() {
  static const char * const names[6] = {
    "None",
    "Equip",
    "Currency",
    "Consum",
    "Invisible",
    nullptr
  };
  return names;
}

inline const char *EnumNameeItemType(eItemType e) {
  if (::flatbuffers::IsOutRange(e, eItemType_None, eItemType_Invisible)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseItemType()[index];
}

enum eItemSubType : int32_t {
  eItemSubType_None = 0,
  eItemSubType_Weapon = 65535,
  eItemSubType_Helm = 65536,
  eItemSubType_Armor = 65537,
  eItemSubType_Belt = 65538,
  eItemSubType_Gloves = 65539,
  eItemSubType_Shoes = 65540,
  eItemSubType_Accessories = 65541,
  eItemSubType_Bronze = 131070,
  eItemSubType_Silver = 131071,
  eItemSubType_Gold = 131072,
  eItemSubType_Platinum = 131073,
  eItemSubType_HPPotion = 196605,
  eItemSubType_MPPotion = 196606,
  eItemSubType_Quest = 196607,
  eItemSubType_MIN = eItemSubType_None,
  eItemSubType_MAX = eItemSubType_Quest
};

inline const eItemSubType (&EnumValueseItemSubType())[15] {
  static const eItemSubType values[] = {
    eItemSubType_None,
    eItemSubType_Weapon,
    eItemSubType_Helm,
    eItemSubType_Armor,
    eItemSubType_Belt,
    eItemSubType_Gloves,
    eItemSubType_Shoes,
    eItemSubType_Accessories,
    eItemSubType_Bronze,
    eItemSubType_Silver,
    eItemSubType_Gold,
    eItemSubType_Platinum,
    eItemSubType_HPPotion,
    eItemSubType_MPPotion,
    eItemSubType_Quest
  };
  return values;
}

inline const char *EnumNameeItemSubType(eItemSubType e) {
  switch (e) {
    case eItemSubType_None: return "None";
    case eItemSubType_Weapon: return "Weapon";
    case eItemSubType_Helm: return "Helm";
    case eItemSubType_Armor: return "Armor";
    case eItemSubType_Belt: return "Belt";
    case eItemSubType_Gloves: return "Gloves";
    case eItemSubType_Shoes: return "Shoes";
    case eItemSubType_Accessories: return "Accessories";
    case eItemSubType_Bronze: return "Bronze";
    case eItemSubType_Silver: return "Silver";
    case eItemSubType_Gold: return "Gold";
    case eItemSubType_Platinum: return "Platinum";
    case eItemSubType_HPPotion: return "HPPotion";
    case eItemSubType_MPPotion: return "MPPotion";
    case eItemSubType_Quest: return "Quest";
    default: return "";
  }
}

enum eItemRank : int32_t {
  eItemRank_E = 0,
  eItemRank_EE = 1,
  eItemRank_EEE = 2,
  eItemRank_D = 3,
  eItemRank_DD = 4,
  eItemRank_DDD = 5,
  eItemRank_C = 6,
  eItemRank_CC = 7,
  eItemRank_CCC = 8,
  eItemRank_B = 9,
  eItemRank_BB = 10,
  eItemRank_BBB = 11,
  eItemRank_A = 12,
  eItemRank_AA = 13,
  eItemRank_AAA = 14,
  eItemRank_S = 15,
  eItemRank_SS = 16,
  eItemRank_SSS = 17,
  eItemRank_MIN = eItemRank_E,
  eItemRank_MAX = eItemRank_SSS
};

inline const eItemRank (&EnumValueseItemRank())[18] {
  static const eItemRank values[] = {
    eItemRank_E,
    eItemRank_EE,
    eItemRank_EEE,
    eItemRank_D,
    eItemRank_DD,
    eItemRank_DDD,
    eItemRank_C,
    eItemRank_CC,
    eItemRank_CCC,
    eItemRank_B,
    eItemRank_BB,
    eItemRank_BBB,
    eItemRank_A,
    eItemRank_AA,
    eItemRank_AAA,
    eItemRank_S,
    eItemRank_SS,
    eItemRank_SSS
  };
  return values;
}

inline const char * const *EnumNameseItemRank() {
  static const char * const names[19] = {
    "E",
    "EE",
    "EEE",
    "D",
    "DD",
    "DDD",
    "C",
    "CC",
    "CCC",
    "B",
    "BB",
    "BBB",
    "A",
    "AA",
    "AAA",
    "S",
    "SS",
    "SSS",
    nullptr
  };
  return names;
}

inline const char *EnumNameeItemRank(eItemRank e) {
  if (::flatbuffers::IsOutRange(e, eItemRank_E, eItemRank_SSS)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseItemRank()[index];
}

enum eCoordAttribute : int32_t {
  eCoordAttribute_None = 0,
  eCoordAttribute_Block = 1,
  eCoordAttribute_MIN = eCoordAttribute_None,
  eCoordAttribute_MAX = eCoordAttribute_Block
};

inline const eCoordAttribute (&EnumValueseCoordAttribute())[2] {
  static const eCoordAttribute values[] = {
    eCoordAttribute_None,
    eCoordAttribute_Block
  };
  return values;
}

inline const char * const *EnumNameseCoordAttribute() {
  static const char * const names[3] = {
    "None",
    "Block",
    nullptr
  };
  return names;
}

inline const char *EnumNameeCoordAttribute(eCoordAttribute e) {
  if (::flatbuffers::IsOutRange(e, eCoordAttribute_None, eCoordAttribute_Block)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseCoordAttribute()[index];
}

enum eTerrrainTytpe : int32_t {
  eTerrrainTytpe_City = 0,
  eTerrrainTytpe_Filed = 1,
  eTerrrainTytpe_Instance = 2,
  eTerrrainTytpe_MIN = eTerrrainTytpe_City,
  eTerrrainTytpe_MAX = eTerrrainTytpe_Instance
};

inline const eTerrrainTytpe (&EnumValueseTerrrainTytpe())[3] {
  static const eTerrrainTytpe values[] = {
    eTerrrainTytpe_City,
    eTerrrainTytpe_Filed,
    eTerrrainTytpe_Instance
  };
  return values;
}

inline const char * const *EnumNameseTerrrainTytpe() {
  static const char * const names[4] = {
    "City",
    "Filed",
    "Instance",
    nullptr
  };
  return names;
}

inline const char *EnumNameeTerrrainTytpe(eTerrrainTytpe e) {
  if (::flatbuffers::IsOutRange(e, eTerrrainTytpe_City, eTerrrainTytpe_Instance)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseTerrrainTytpe()[index];
}

enum eAbilityTYpe : int32_t {
  eAbilityTYpe_Attack1 = 0,
  eAbilityTYpe_Attack2 = 1,
  eAbilityTYpe_Attack3 = 2,
  eAbilityTYpe_Attack4 = 3,
  eAbilityTYpe_Attack5 = 4,
  eAbilityTYpe_Attack6 = 5,
  eAbilityTYpe_Defense1 = 6,
  eAbilityTYpe_Defense2 = 7,
  eAbilityTYpe_Defense3 = 8,
  eAbilityTYpe_Defense4 = 9,
  eAbilityTYpe_Defense5 = 10,
  eAbilityTYpe_Defense6 = 11,
  eAbilityTYpe_MonsterAttack = 12,
  eAbilityTYpe_MonsterDefense = 13,
  eAbilityTYpe_Cri = 14,
  eAbilityTYpe_CriDef = 15,
  eAbilityTYpe_CriAttack = 16,
  eAbilityTYpe_Hit = 17,
  eAbilityTYpe_HitDef = 18,
  eAbilityTYpe_MoveSpeed = 19,
  eAbilityTYpe_Hp = 20,
  eAbilityTYpe_Mp = 21,
  eAbilityTYpe_RegenHp = 22,
  eAbilityTYpe_RegenMp = 23,
  eAbilityTYpe_DrainHp = 24,
  eAbilityTYpe_DrainMp = 25,
  eAbilityTYpe_Str = 26,
  eAbilityTYpe_Dex = 27,
  eAbilityTYpe_Int = 28,
  eAbilityTYpe_Con = 29,
  eAbilityTYpe_Immovable = 30,
  eAbilityTYpe_Stun = 31,
  eAbilityTYpe_MIN = eAbilityTYpe_Attack1,
  eAbilityTYpe_MAX = eAbilityTYpe_Stun
};

inline const eAbilityTYpe (&EnumValueseAbilityTYpe())[32] {
  static const eAbilityTYpe values[] = {
    eAbilityTYpe_Attack1,
    eAbilityTYpe_Attack2,
    eAbilityTYpe_Attack3,
    eAbilityTYpe_Attack4,
    eAbilityTYpe_Attack5,
    eAbilityTYpe_Attack6,
    eAbilityTYpe_Defense1,
    eAbilityTYpe_Defense2,
    eAbilityTYpe_Defense3,
    eAbilityTYpe_Defense4,
    eAbilityTYpe_Defense5,
    eAbilityTYpe_Defense6,
    eAbilityTYpe_MonsterAttack,
    eAbilityTYpe_MonsterDefense,
    eAbilityTYpe_Cri,
    eAbilityTYpe_CriDef,
    eAbilityTYpe_CriAttack,
    eAbilityTYpe_Hit,
    eAbilityTYpe_HitDef,
    eAbilityTYpe_MoveSpeed,
    eAbilityTYpe_Hp,
    eAbilityTYpe_Mp,
    eAbilityTYpe_RegenHp,
    eAbilityTYpe_RegenMp,
    eAbilityTYpe_DrainHp,
    eAbilityTYpe_DrainMp,
    eAbilityTYpe_Str,
    eAbilityTYpe_Dex,
    eAbilityTYpe_Int,
    eAbilityTYpe_Con,
    eAbilityTYpe_Immovable,
    eAbilityTYpe_Stun
  };
  return values;
}

inline const char * const *EnumNameseAbilityTYpe() {
  static const char * const names[33] = {
    "Attack1",
    "Attack2",
    "Attack3",
    "Attack4",
    "Attack5",
    "Attack6",
    "Defense1",
    "Defense2",
    "Defense3",
    "Defense4",
    "Defense5",
    "Defense6",
    "MonsterAttack",
    "MonsterDefense",
    "Cri",
    "CriDef",
    "CriAttack",
    "Hit",
    "HitDef",
    "MoveSpeed",
    "Hp",
    "Mp",
    "RegenHp",
    "RegenMp",
    "DrainHp",
    "DrainMp",
    "Str",
    "Dex",
    "Int",
    "Con",
    "Immovable",
    "Stun",
    nullptr
  };
  return names;
}

inline const char *EnumNameeAbilityTYpe(eAbilityTYpe e) {
  if (::flatbuffers::IsOutRange(e, eAbilityTYpe_Attack1, eAbilityTYpe_Stun)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseAbilityTYpe()[index];
}

enum eAiType : int32_t {
  eAiType_None = -1,
  eAiType_Aggressive = 0,
  eAiType_NonAggressive = 1,
  eAiType_Boss = 2,
  eAiType_MIN = eAiType_None,
  eAiType_MAX = eAiType_Boss
};

inline const eAiType (&EnumValueseAiType())[4] {
  static const eAiType values[] = {
    eAiType_None,
    eAiType_Aggressive,
    eAiType_NonAggressive,
    eAiType_Boss
  };
  return values;
}

inline const char * const *EnumNameseAiType() {
  static const char * const names[5] = {
    "None",
    "Aggressive",
    "NonAggressive",
    "Boss",
    nullptr
  };
  return names;
}

inline const char *EnumNameeAiType(eAiType e) {
  if (::flatbuffers::IsOutRange(e, eAiType_None, eAiType_Boss)) return "";
  const size_t index = static_cast<size_t>(e) - static_cast<size_t>(eAiType_None);
  return EnumNameseAiType()[index];
}

enum eMapAttribute : int32_t {
  eMapAttribute_None = 0,
  eMapAttribute_NotTeleport = 1,
  eMapAttribute_NotPvp = 2,
  eMapAttribute_NotSkill = 4,
  eMapAttribute_MIN = eMapAttribute_None,
  eMapAttribute_MAX = eMapAttribute_NotSkill
};

inline const eMapAttribute (&EnumValueseMapAttribute())[4] {
  static const eMapAttribute values[] = {
    eMapAttribute_None,
    eMapAttribute_NotTeleport,
    eMapAttribute_NotPvp,
    eMapAttribute_NotSkill
  };
  return values;
}

inline const char * const *EnumNameseMapAttribute() {
  static const char * const names[6] = {
    "None",
    "NotTeleport",
    "NotPvp",
    "",
    "NotSkill",
    nullptr
  };
  return names;
}

inline const char *EnumNameeMapAttribute(eMapAttribute e) {
  if (::flatbuffers::IsOutRange(e, eMapAttribute_None, eMapAttribute_NotSkill)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseMapAttribute()[index];
}

enum eGuildMemberGrade : int32_t {
  eGuildMemberGrade_None = 0,
  eGuildMemberGrade_Normal = 1,
  eGuildMemberGrade_SubMaster = 2,
  eGuildMemberGrade_Master = 99,
  eGuildMemberGrade_MIN = eGuildMemberGrade_None,
  eGuildMemberGrade_MAX = eGuildMemberGrade_Master
};

inline const eGuildMemberGrade (&EnumValueseGuildMemberGrade())[4] {
  static const eGuildMemberGrade values[] = {
    eGuildMemberGrade_None,
    eGuildMemberGrade_Normal,
    eGuildMemberGrade_SubMaster,
    eGuildMemberGrade_Master
  };
  return values;
}

inline const char *EnumNameeGuildMemberGrade(eGuildMemberGrade e) {
  switch (e) {
    case eGuildMemberGrade_None: return "None";
    case eGuildMemberGrade_Normal: return "Normal";
    case eGuildMemberGrade_SubMaster: return "SubMaster";
    case eGuildMemberGrade_Master: return "Master";
    default: return "";
  }
}

}  // namespace fb

#endif  // FLATBUFFERS_GENERATED_COMMONENUM_FB_H_
