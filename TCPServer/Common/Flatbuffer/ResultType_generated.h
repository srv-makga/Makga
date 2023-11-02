// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_RESULTTYPE_FB_H_
#define FLATBUFFERS_GENERATED_RESULTTYPE_FB_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
              FLATBUFFERS_VERSION_MINOR == 1 &&
              FLATBUFFERS_VERSION_REVISION == 21,
             "Non-compatible flatbuffers version included");

namespace fb {

enum eResult : int32_t {
  eResult_Success = 0,
  eResult_Fail = 1,
  eResult_BufferLackSpace = 2,
  eResult_BufferFailWrite = 3,
  eResult_BufferFailRead = 4,
  eResult_InvalidParameter = 5,
  eResult_InvalidIndex = 6,
  eResult_NotFoundServer = 7,
  eResult_ActorNotFound = 8,
  eResult_ItemNotFound = 9,
  eResult_ItemStackFull = 10,
  eResult_ItemNotEnough = 11,
  eResult_ItemUidDuplicate = 12,
  eResult_ItemIdxDuplicate = 13,
  eResult_ItemSameSkinIndex = 14,
  eResult_InvenFull = 15,
  eResult_CurrencyLackGold = 16,
  eResult_MIN = eResult_Success,
  eResult_MAX = eResult_CurrencyLackGold
};

inline const eResult (&EnumValueseResult())[17] {
  static const eResult values[] = {
    eResult_Success,
    eResult_Fail,
    eResult_BufferLackSpace,
    eResult_BufferFailWrite,
    eResult_BufferFailRead,
    eResult_InvalidParameter,
    eResult_InvalidIndex,
    eResult_NotFoundServer,
    eResult_ActorNotFound,
    eResult_ItemNotFound,
    eResult_ItemStackFull,
    eResult_ItemNotEnough,
    eResult_ItemUidDuplicate,
    eResult_ItemIdxDuplicate,
    eResult_ItemSameSkinIndex,
    eResult_InvenFull,
    eResult_CurrencyLackGold
  };
  return values;
}

inline const char * const *EnumNameseResult() {
  static const char * const names[18] = {
    "Success",
    "Fail",
    "BufferLackSpace",
    "BufferFailWrite",
    "BufferFailRead",
    "InvalidParameter",
    "InvalidIndex",
    "NotFoundServer",
    "ActorNotFound",
    "ItemNotFound",
    "ItemStackFull",
    "ItemNotEnough",
    "ItemUidDuplicate",
    "ItemIdxDuplicate",
    "ItemSameSkinIndex",
    "InvenFull",
    "CurrencyLackGold",
    nullptr
  };
  return names;
}

inline const char *EnumNameeResult(eResult e) {
  if (::flatbuffers::IsOutRange(e, eResult_Success, eResult_CurrencyLackGold)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNameseResult()[index];
}

}  // namespace fb

#endif  // FLATBUFFERS_GENERATED_RESULTTYPE_FB_H_
