#pragma once

#include <string>

namespace core {
namespace db {
class Statement {
public:
	virtual void Bind(int32_t _sequence, int8_t& _value) = 0;
	virtual void Bind(int32_t _sequence, uint8_t& _value) = 0;
	virtual void Bind(int32_t _sequence, int16_t& _value) = 0;
	virtual void Bind(int32_t _sequence, uint16_t& _value) = 0;
	virtual void Bind(int32_t _sequence, int32_t& _value) = 0;
	virtual void Bind(int32_t _sequence, uint32_t& _value) = 0;
	virtual void Bind(int32_t _sequence, int64_t& _value) = 0;
	virtual void Bind(int32_t _sequence, uint64_t& _value) = 0;
	virtual void Bind(int32_t _sequence, float& _value) = 0;
	virtual void Bind(int32_t _sequence, double& _value) = 0;
	virtual void Bind(int32_t _sequence, std::string& _value) = 0;
	virtual void Bind(int32_t _sequence, bool& _value) = 0;

	virtual bool Get(int32_t _sequence, int8_t& _value) = 0;
	virtual bool Get(int32_t _sequence, uint8_t& _value) = 0;
	virtual bool Get(int32_t _sequence, int16_t& _value) = 0;
	virtual bool Get(int32_t _sequence, uint16_t& _value) = 0;
	virtual bool Get(int32_t _sequence, int32_t& _value) = 0;
	virtual bool Get(int32_t _sequence, uint32_t& _value) = 0;
	virtual bool Get(int32_t _sequence, int64_t& _value) = 0;
	virtual bool Get(int32_t _sequence, uint64_t& _value) = 0;
	virtual bool Get(int32_t _sequence, float& _value) = 0;
	virtual bool Get(int32_t _sequence, double& _value) = 0;
	virtual bool Get(int32_t _sequence, std::string& _value) = 0;
	virtual bool Get(int32_t _sequence, bool& _value) = 0;

	virtual bool Execute() = 0;
	virtual bool Fetch() = 0;
	virtual bool IsMore() = 0;
	virtual int RowCount() const = 0;
};
} // namespace db
} // namespace core
