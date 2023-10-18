// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_SERVERTOGLOBALTABLE_FB_GLOBAL_H_
#define FLATBUFFERS_GENERATED_SERVERTOGLOBALTABLE_FB_GLOBAL_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
              FLATBUFFERS_VERSION_MINOR == 1 &&
              FLATBUFFERS_VERSION_REVISION == 21,
             "Non-compatible flatbuffers version included");

namespace fb {
namespace global {

struct Send_Reg;
struct Send_RegBuilder;
struct Send_RegT;

struct Send_Ping;
struct Send_PingBuilder;
struct Send_PingT;

struct Recv_Reg;
struct Recv_RegBuilder;
struct Recv_RegT;

struct Recv_Ping;
struct Recv_PingBuilder;
struct Recv_PingT;

struct Send_RegT : public ::flatbuffers::NativeTable {
  typedef Send_Reg TableType;
  uint32_t server_id = 0;
};

struct Send_Reg FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef Send_RegT NativeTableType;
  typedef Send_RegBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SERVER_ID = 4
  };
  uint32_t server_id() const {
    return GetField<uint32_t>(VT_SERVER_ID, 0);
  }
  bool mutate_server_id(uint32_t _server_id = 0) {
    return SetField<uint32_t>(VT_SERVER_ID, _server_id, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_SERVER_ID, 4) &&
           verifier.EndTable();
  }
  Send_RegT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(Send_RegT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<Send_Reg> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Send_RegT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct Send_RegBuilder {
  typedef Send_Reg Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_server_id(uint32_t server_id) {
    fbb_.AddElement<uint32_t>(Send_Reg::VT_SERVER_ID, server_id, 0);
  }
  explicit Send_RegBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Send_Reg> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Send_Reg>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Send_Reg> CreateSend_Reg(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t server_id = 0) {
  Send_RegBuilder builder_(_fbb);
  builder_.add_server_id(server_id);
  return builder_.Finish();
}

::flatbuffers::Offset<Send_Reg> CreateSend_Reg(::flatbuffers::FlatBufferBuilder &_fbb, const Send_RegT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct Send_PingT : public ::flatbuffers::NativeTable {
  typedef Send_Ping TableType;
  int64_t time = 0;
};

struct Send_Ping FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef Send_PingT NativeTableType;
  typedef Send_PingBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TIME = 4
  };
  int64_t time() const {
    return GetField<int64_t>(VT_TIME, 0);
  }
  bool mutate_time(int64_t _time = 0) {
    return SetField<int64_t>(VT_TIME, _time, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int64_t>(verifier, VT_TIME, 8) &&
           verifier.EndTable();
  }
  Send_PingT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(Send_PingT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<Send_Ping> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Send_PingT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct Send_PingBuilder {
  typedef Send_Ping Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_time(int64_t time) {
    fbb_.AddElement<int64_t>(Send_Ping::VT_TIME, time, 0);
  }
  explicit Send_PingBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Send_Ping> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Send_Ping>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Send_Ping> CreateSend_Ping(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int64_t time = 0) {
  Send_PingBuilder builder_(_fbb);
  builder_.add_time(time);
  return builder_.Finish();
}

::flatbuffers::Offset<Send_Ping> CreateSend_Ping(::flatbuffers::FlatBufferBuilder &_fbb, const Send_PingT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct Recv_RegT : public ::flatbuffers::NativeTable {
  typedef Recv_Reg TableType;
  fb::eResult result = fb::eResult_Success;
};

struct Recv_Reg FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef Recv_RegT NativeTableType;
  typedef Recv_RegBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_RESULT = 4
  };
  fb::eResult result() const {
    return static_cast<fb::eResult>(GetField<int32_t>(VT_RESULT, 0));
  }
  bool mutate_result(fb::eResult _result = static_cast<fb::eResult>(0)) {
    return SetField<int32_t>(VT_RESULT, static_cast<int32_t>(_result), 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_RESULT, 4) &&
           verifier.EndTable();
  }
  Recv_RegT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(Recv_RegT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<Recv_Reg> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Recv_RegT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct Recv_RegBuilder {
  typedef Recv_Reg Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_result(fb::eResult result) {
    fbb_.AddElement<int32_t>(Recv_Reg::VT_RESULT, static_cast<int32_t>(result), 0);
  }
  explicit Recv_RegBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Recv_Reg> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Recv_Reg>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Recv_Reg> CreateRecv_Reg(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    fb::eResult result = fb::eResult_Success) {
  Recv_RegBuilder builder_(_fbb);
  builder_.add_result(result);
  return builder_.Finish();
}

::flatbuffers::Offset<Recv_Reg> CreateRecv_Reg(::flatbuffers::FlatBufferBuilder &_fbb, const Recv_RegT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct Recv_PingT : public ::flatbuffers::NativeTable {
  typedef Recv_Ping TableType;
  int64_t time = 0;
};

struct Recv_Ping FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef Recv_PingT NativeTableType;
  typedef Recv_PingBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TIME = 4
  };
  int64_t time() const {
    return GetField<int64_t>(VT_TIME, 0);
  }
  bool mutate_time(int64_t _time = 0) {
    return SetField<int64_t>(VT_TIME, _time, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int64_t>(verifier, VT_TIME, 8) &&
           verifier.EndTable();
  }
  Recv_PingT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(Recv_PingT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<Recv_Ping> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Recv_PingT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct Recv_PingBuilder {
  typedef Recv_Ping Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_time(int64_t time) {
    fbb_.AddElement<int64_t>(Recv_Ping::VT_TIME, time, 0);
  }
  explicit Recv_PingBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Recv_Ping> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Recv_Ping>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Recv_Ping> CreateRecv_Ping(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int64_t time = 0) {
  Recv_PingBuilder builder_(_fbb);
  builder_.add_time(time);
  return builder_.Finish();
}

::flatbuffers::Offset<Recv_Ping> CreateRecv_Ping(::flatbuffers::FlatBufferBuilder &_fbb, const Recv_PingT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline Send_RegT *Send_Reg::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<Send_RegT>(new Send_RegT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void Send_Reg::UnPackTo(Send_RegT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = server_id(); _o->server_id = _e; }
}

inline ::flatbuffers::Offset<Send_Reg> Send_Reg::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Send_RegT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateSend_Reg(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<Send_Reg> CreateSend_Reg(::flatbuffers::FlatBufferBuilder &_fbb, const Send_RegT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const Send_RegT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _server_id = _o->server_id;
  return fb::global::CreateSend_Reg(
      _fbb,
      _server_id);
}

inline Send_PingT *Send_Ping::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<Send_PingT>(new Send_PingT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void Send_Ping::UnPackTo(Send_PingT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = time(); _o->time = _e; }
}

inline ::flatbuffers::Offset<Send_Ping> Send_Ping::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Send_PingT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateSend_Ping(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<Send_Ping> CreateSend_Ping(::flatbuffers::FlatBufferBuilder &_fbb, const Send_PingT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const Send_PingT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _time = _o->time;
  return fb::global::CreateSend_Ping(
      _fbb,
      _time);
}

inline Recv_RegT *Recv_Reg::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<Recv_RegT>(new Recv_RegT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void Recv_Reg::UnPackTo(Recv_RegT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = result(); _o->result = _e; }
}

inline ::flatbuffers::Offset<Recv_Reg> Recv_Reg::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Recv_RegT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateRecv_Reg(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<Recv_Reg> CreateRecv_Reg(::flatbuffers::FlatBufferBuilder &_fbb, const Recv_RegT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const Recv_RegT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _result = _o->result;
  return fb::global::CreateRecv_Reg(
      _fbb,
      _result);
}

inline Recv_PingT *Recv_Ping::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<Recv_PingT>(new Recv_PingT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void Recv_Ping::UnPackTo(Recv_PingT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = time(); _o->time = _e; }
}

inline ::flatbuffers::Offset<Recv_Ping> Recv_Ping::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Recv_PingT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateRecv_Ping(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<Recv_Ping> CreateRecv_Ping(::flatbuffers::FlatBufferBuilder &_fbb, const Recv_PingT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const Recv_PingT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _time = _o->time;
  return fb::global::CreateRecv_Ping(
      _fbb,
      _time);
}

}  // namespace global
}  // namespace fb

#endif  // FLATBUFFERS_GENERATED_SERVERTOGLOBALTABLE_FB_GLOBAL_H_
