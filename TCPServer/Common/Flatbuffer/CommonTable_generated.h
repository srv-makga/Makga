// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_COMMONTABLE_FB_H_
#define FLATBUFFERS_GENERATED_COMMONTABLE_FB_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
              FLATBUFFERS_VERSION_MINOR == 1 &&
              FLATBUFFERS_VERSION_REVISION == 21,
             "Non-compatible flatbuffers version included");

namespace fb {

struct ServerInfo;
struct ServerInfoBuilder;
struct ServerInfoT;

struct Vector3;
struct Vector3Builder;
struct Vector3T;

struct Position;
struct PositionBuilder;
struct PositionT;

struct PartyCreateOption;
struct PartyCreateOptionBuilder;
struct PartyCreateOptionT;

struct GuildCreateOption;
struct GuildCreateOptionBuilder;
struct GuildCreateOptionT;

struct ServerInfoT : public ::flatbuffers::NativeTable {
  typedef ServerInfo TableType;
  int32_t server_id = 0;
  std::string ip{};
  std::vector<uint16_t> port{};
};

struct ServerInfo FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef ServerInfoT NativeTableType;
  typedef ServerInfoBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SERVER_ID = 4,
    VT_IP = 6,
    VT_PORT = 8
  };
  int32_t server_id() const {
    return GetField<int32_t>(VT_SERVER_ID, 0);
  }
  bool mutate_server_id(int32_t _server_id = 0) {
    return SetField<int32_t>(VT_SERVER_ID, _server_id, 0);
  }
  const ::flatbuffers::String *ip() const {
    return GetPointer<const ::flatbuffers::String *>(VT_IP);
  }
  ::flatbuffers::String *mutable_ip() {
    return GetPointer<::flatbuffers::String *>(VT_IP);
  }
  const ::flatbuffers::Vector<uint16_t> *port() const {
    return GetPointer<const ::flatbuffers::Vector<uint16_t> *>(VT_PORT);
  }
  ::flatbuffers::Vector<uint16_t> *mutable_port() {
    return GetPointer<::flatbuffers::Vector<uint16_t> *>(VT_PORT);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_SERVER_ID, 4) &&
           VerifyOffset(verifier, VT_IP) &&
           verifier.VerifyString(ip()) &&
           VerifyOffset(verifier, VT_PORT) &&
           verifier.VerifyVector(port()) &&
           verifier.EndTable();
  }
  ServerInfoT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(ServerInfoT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<ServerInfo> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const ServerInfoT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct ServerInfoBuilder {
  typedef ServerInfo Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_server_id(int32_t server_id) {
    fbb_.AddElement<int32_t>(ServerInfo::VT_SERVER_ID, server_id, 0);
  }
  void add_ip(::flatbuffers::Offset<::flatbuffers::String> ip) {
    fbb_.AddOffset(ServerInfo::VT_IP, ip);
  }
  void add_port(::flatbuffers::Offset<::flatbuffers::Vector<uint16_t>> port) {
    fbb_.AddOffset(ServerInfo::VT_PORT, port);
  }
  explicit ServerInfoBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<ServerInfo> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<ServerInfo>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<ServerInfo> CreateServerInfo(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t server_id = 0,
    ::flatbuffers::Offset<::flatbuffers::String> ip = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint16_t>> port = 0) {
  ServerInfoBuilder builder_(_fbb);
  builder_.add_port(port);
  builder_.add_ip(ip);
  builder_.add_server_id(server_id);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<ServerInfo> CreateServerInfoDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t server_id = 0,
    const char *ip = nullptr,
    const std::vector<uint16_t> *port = nullptr) {
  auto ip__ = ip ? _fbb.CreateString(ip) : 0;
  auto port__ = port ? _fbb.CreateVector<uint16_t>(*port) : 0;
  return fb::CreateServerInfo(
      _fbb,
      server_id,
      ip__,
      port__);
}

::flatbuffers::Offset<ServerInfo> CreateServerInfo(::flatbuffers::FlatBufferBuilder &_fbb, const ServerInfoT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct Vector3T : public ::flatbuffers::NativeTable {
  typedef Vector3 TableType;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};

struct Vector3 FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef Vector3T NativeTableType;
  typedef Vector3Builder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  bool mutate_x(float _x = 0.0f) {
    return SetField<float>(VT_X, _x, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  bool mutate_y(float _y = 0.0f) {
    return SetField<float>(VT_Y, _y, 0.0f);
  }
  float z() const {
    return GetField<float>(VT_Z, 0.0f);
  }
  bool mutate_z(float _z = 0.0f) {
    return SetField<float>(VT_Z, _z, 0.0f);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_Z, 4) &&
           verifier.EndTable();
  }
  Vector3T *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(Vector3T *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<Vector3> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Vector3T* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct Vector3Builder {
  typedef Vector3 Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(Vector3::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Vector3::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(Vector3::VT_Z, z, 0.0f);
  }
  explicit Vector3Builder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Vector3> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Vector3>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Vector3> CreateVector3(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f) {
  Vector3Builder builder_(_fbb);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

::flatbuffers::Offset<Vector3> CreateVector3(::flatbuffers::FlatBufferBuilder &_fbb, const Vector3T *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct PositionT : public ::flatbuffers::NativeTable {
  typedef Position TableType;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float direction = 0.0f;
};

struct Position FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PositionT NativeTableType;
  typedef PositionBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8,
    VT_DIRECTION = 10
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  bool mutate_x(float _x = 0.0f) {
    return SetField<float>(VT_X, _x, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  bool mutate_y(float _y = 0.0f) {
    return SetField<float>(VT_Y, _y, 0.0f);
  }
  float z() const {
    return GetField<float>(VT_Z, 0.0f);
  }
  bool mutate_z(float _z = 0.0f) {
    return SetField<float>(VT_Z, _z, 0.0f);
  }
  float direction() const {
    return GetField<float>(VT_DIRECTION, 0.0f);
  }
  bool mutate_direction(float _direction = 0.0f) {
    return SetField<float>(VT_DIRECTION, _direction, 0.0f);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_Z, 4) &&
           VerifyField<float>(verifier, VT_DIRECTION, 4) &&
           verifier.EndTable();
  }
  PositionT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(PositionT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<Position> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const PositionT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct PositionBuilder {
  typedef Position Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(Position::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Position::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(Position::VT_Z, z, 0.0f);
  }
  void add_direction(float direction) {
    fbb_.AddElement<float>(Position::VT_DIRECTION, direction, 0.0f);
  }
  explicit PositionBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Position> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Position>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Position> CreatePosition(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f,
    float direction = 0.0f) {
  PositionBuilder builder_(_fbb);
  builder_.add_direction(direction);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

::flatbuffers::Offset<Position> CreatePosition(::flatbuffers::FlatBufferBuilder &_fbb, const PositionT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct PartyCreateOptionT : public ::flatbuffers::NativeTable {
  typedef PartyCreateOption TableType;
  std::string notice{};
};

struct PartyCreateOption FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PartyCreateOptionT NativeTableType;
  typedef PartyCreateOptionBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NOTICE = 4
  };
  const ::flatbuffers::String *notice() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NOTICE);
  }
  ::flatbuffers::String *mutable_notice() {
    return GetPointer<::flatbuffers::String *>(VT_NOTICE);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NOTICE) &&
           verifier.VerifyString(notice()) &&
           verifier.EndTable();
  }
  PartyCreateOptionT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(PartyCreateOptionT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<PartyCreateOption> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const PartyCreateOptionT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct PartyCreateOptionBuilder {
  typedef PartyCreateOption Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_notice(::flatbuffers::Offset<::flatbuffers::String> notice) {
    fbb_.AddOffset(PartyCreateOption::VT_NOTICE, notice);
  }
  explicit PartyCreateOptionBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<PartyCreateOption> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<PartyCreateOption>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<PartyCreateOption> CreatePartyCreateOption(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> notice = 0) {
  PartyCreateOptionBuilder builder_(_fbb);
  builder_.add_notice(notice);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<PartyCreateOption> CreatePartyCreateOptionDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *notice = nullptr) {
  auto notice__ = notice ? _fbb.CreateString(notice) : 0;
  return fb::CreatePartyCreateOption(
      _fbb,
      notice__);
}

::flatbuffers::Offset<PartyCreateOption> CreatePartyCreateOption(::flatbuffers::FlatBufferBuilder &_fbb, const PartyCreateOptionT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

struct GuildCreateOptionT : public ::flatbuffers::NativeTable {
  typedef GuildCreateOption TableType;
  std::string name{};
  uint64_t mark = 0;
  std::string notice{};
};

struct GuildCreateOption FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef GuildCreateOptionT NativeTableType;
  typedef GuildCreateOptionBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_MARK = 6,
    VT_NOTICE = 8
  };
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  ::flatbuffers::String *mutable_name() {
    return GetPointer<::flatbuffers::String *>(VT_NAME);
  }
  uint64_t mark() const {
    return GetField<uint64_t>(VT_MARK, 0);
  }
  bool mutate_mark(uint64_t _mark = 0) {
    return SetField<uint64_t>(VT_MARK, _mark, 0);
  }
  const ::flatbuffers::String *notice() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NOTICE);
  }
  ::flatbuffers::String *mutable_notice() {
    return GetPointer<::flatbuffers::String *>(VT_NOTICE);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyField<uint64_t>(verifier, VT_MARK, 8) &&
           VerifyOffset(verifier, VT_NOTICE) &&
           verifier.VerifyString(notice()) &&
           verifier.EndTable();
  }
  GuildCreateOptionT *UnPack(const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(GuildCreateOptionT *_o, const ::flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static ::flatbuffers::Offset<GuildCreateOption> Pack(::flatbuffers::FlatBufferBuilder &_fbb, const GuildCreateOptionT* _o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct GuildCreateOptionBuilder {
  typedef GuildCreateOption Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(GuildCreateOption::VT_NAME, name);
  }
  void add_mark(uint64_t mark) {
    fbb_.AddElement<uint64_t>(GuildCreateOption::VT_MARK, mark, 0);
  }
  void add_notice(::flatbuffers::Offset<::flatbuffers::String> notice) {
    fbb_.AddOffset(GuildCreateOption::VT_NOTICE, notice);
  }
  explicit GuildCreateOptionBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<GuildCreateOption> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<GuildCreateOption>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<GuildCreateOption> CreateGuildCreateOption(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    uint64_t mark = 0,
    ::flatbuffers::Offset<::flatbuffers::String> notice = 0) {
  GuildCreateOptionBuilder builder_(_fbb);
  builder_.add_mark(mark);
  builder_.add_notice(notice);
  builder_.add_name(name);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<GuildCreateOption> CreateGuildCreateOptionDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    uint64_t mark = 0,
    const char *notice = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto notice__ = notice ? _fbb.CreateString(notice) : 0;
  return fb::CreateGuildCreateOption(
      _fbb,
      name__,
      mark,
      notice__);
}

::flatbuffers::Offset<GuildCreateOption> CreateGuildCreateOption(::flatbuffers::FlatBufferBuilder &_fbb, const GuildCreateOptionT *_o, const ::flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline ServerInfoT *ServerInfo::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<ServerInfoT>(new ServerInfoT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void ServerInfo::UnPackTo(ServerInfoT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = server_id(); _o->server_id = _e; }
  { auto _e = ip(); if (_e) _o->ip = _e->str(); }
  { auto _e = port(); if (_e) { _o->port.resize(_e->size()); for (::flatbuffers::uoffset_t _i = 0; _i < _e->size(); _i++) { _o->port[_i] = _e->Get(_i); } } else { _o->port.resize(0); } }
}

inline ::flatbuffers::Offset<ServerInfo> ServerInfo::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const ServerInfoT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateServerInfo(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<ServerInfo> CreateServerInfo(::flatbuffers::FlatBufferBuilder &_fbb, const ServerInfoT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const ServerInfoT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _server_id = _o->server_id;
  auto _ip = _o->ip.empty() ? 0 : _fbb.CreateString(_o->ip);
  auto _port = _o->port.size() ? _fbb.CreateVector(_o->port) : 0;
  return fb::CreateServerInfo(
      _fbb,
      _server_id,
      _ip,
      _port);
}

inline Vector3T *Vector3::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<Vector3T>(new Vector3T());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void Vector3::UnPackTo(Vector3T *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = x(); _o->x = _e; }
  { auto _e = y(); _o->y = _e; }
  { auto _e = z(); _o->z = _e; }
}

inline ::flatbuffers::Offset<Vector3> Vector3::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const Vector3T* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateVector3(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<Vector3> CreateVector3(::flatbuffers::FlatBufferBuilder &_fbb, const Vector3T *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const Vector3T* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _x = _o->x;
  auto _y = _o->y;
  auto _z = _o->z;
  return fb::CreateVector3(
      _fbb,
      _x,
      _y,
      _z);
}

inline PositionT *Position::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<PositionT>(new PositionT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void Position::UnPackTo(PositionT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = x(); _o->x = _e; }
  { auto _e = y(); _o->y = _e; }
  { auto _e = z(); _o->z = _e; }
  { auto _e = direction(); _o->direction = _e; }
}

inline ::flatbuffers::Offset<Position> Position::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const PositionT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreatePosition(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<Position> CreatePosition(::flatbuffers::FlatBufferBuilder &_fbb, const PositionT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const PositionT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _x = _o->x;
  auto _y = _o->y;
  auto _z = _o->z;
  auto _direction = _o->direction;
  return fb::CreatePosition(
      _fbb,
      _x,
      _y,
      _z,
      _direction);
}

inline PartyCreateOptionT *PartyCreateOption::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<PartyCreateOptionT>(new PartyCreateOptionT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void PartyCreateOption::UnPackTo(PartyCreateOptionT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = notice(); if (_e) _o->notice = _e->str(); }
}

inline ::flatbuffers::Offset<PartyCreateOption> PartyCreateOption::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const PartyCreateOptionT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreatePartyCreateOption(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<PartyCreateOption> CreatePartyCreateOption(::flatbuffers::FlatBufferBuilder &_fbb, const PartyCreateOptionT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const PartyCreateOptionT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _notice = _o->notice.empty() ? 0 : _fbb.CreateString(_o->notice);
  return fb::CreatePartyCreateOption(
      _fbb,
      _notice);
}

inline GuildCreateOptionT *GuildCreateOption::UnPack(const ::flatbuffers::resolver_function_t *_resolver) const {
  auto _o = std::unique_ptr<GuildCreateOptionT>(new GuildCreateOptionT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void GuildCreateOption::UnPackTo(GuildCreateOptionT *_o, const ::flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = name(); if (_e) _o->name = _e->str(); }
  { auto _e = mark(); _o->mark = _e; }
  { auto _e = notice(); if (_e) _o->notice = _e->str(); }
}

inline ::flatbuffers::Offset<GuildCreateOption> GuildCreateOption::Pack(::flatbuffers::FlatBufferBuilder &_fbb, const GuildCreateOptionT* _o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  return CreateGuildCreateOption(_fbb, _o, _rehasher);
}

inline ::flatbuffers::Offset<GuildCreateOption> CreateGuildCreateOption(::flatbuffers::FlatBufferBuilder &_fbb, const GuildCreateOptionT *_o, const ::flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { ::flatbuffers::FlatBufferBuilder *__fbb; const GuildCreateOptionT* __o; const ::flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _name = _o->name.empty() ? 0 : _fbb.CreateString(_o->name);
  auto _mark = _o->mark;
  auto _notice = _o->notice.empty() ? 0 : _fbb.CreateString(_o->notice);
  return fb::CreateGuildCreateOption(
      _fbb,
      _name,
      _mark,
      _notice);
}

}  // namespace fb

#endif  // FLATBUFFERS_GENERATED_COMMONTABLE_FB_H_
