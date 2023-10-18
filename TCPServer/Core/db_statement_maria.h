#pragma once

#include "db_statement.h"
#include "mariadb/conncpp/PreparedStatement.hpp"
#include "mariadb/conncpp/ResultSet.hpp"

namespace core {
namespace db {
namespace maria {
class MariaStatement : public Statement
{
public:
	MariaStatement(sql::PreparedStatement* _stmt);
	virtual ~MariaStatement();

	void Bind(int32_t _sequence, int8_t& _value) override;
	void Bind(int32_t _sequence, uint8_t& _value) override;
	void Bind(int32_t _sequence, int16_t& _value) override;
	void Bind(int32_t _sequence, uint16_t& _value) override;
	void Bind(int32_t _sequence, int32_t& _value) override;
	void Bind(int32_t _sequence, uint32_t& _value) override;
	void Bind(int32_t _sequence, int64_t& _value) override;
	void Bind(int32_t _sequence, uint64_t& _value) override;
	void Bind(int32_t _sequence, float& _value) override;
	void Bind(int32_t _sequence, double& _value) override;
	void Bind(int32_t _sequence, std::string& _value) override;
	void Bind(int32_t _sequence, bool& _value) override;

	bool Get(int32_t _sequence, int8_t& _value) override;
	bool Get(int32_t _sequence, uint8_t& _value) override;
	bool Get(int32_t _sequence, int16_t& _value) override;
	bool Get(int32_t _sequence, uint16_t& _value) override;
	bool Get(int32_t _sequence, int32_t& _value) override;
	bool Get(int32_t _sequence, uint32_t& _value) override;
	bool Get(int32_t _sequence, int64_t& _value) override;
	bool Get(int32_t _sequence, uint64_t& _value) override;
	bool Get(int32_t _sequence, float& _value) override;
	bool Get(int32_t _sequence, double& _value) override;
	bool Get(int32_t _sequence, std::string& _value) override;
	bool Get(int32_t _sequence, bool& _value) override;

	bool Execute() override;
	bool Fetch() override;
	bool IsMore() override;
	int RowCount() const override;

	_NODISCARD sql::PreparedStatement* operator->() const noexcept
	{
		return m_stmt;
	}

private:
	sql::PreparedStatement* m_stmt;
	sql::ResultSet* m_result;
};
} // namespace maria
} // namespace db
} // namespace core