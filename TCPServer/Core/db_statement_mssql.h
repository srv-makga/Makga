#include "db_statement.h"
#include <type_traits>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

namespace core {
namespace db {
namespace mssql {
class MssqlStatement : public Statement
{
public:
	MssqlStatement(SQLHSTMT _stmt);
	virtual ~MssqlStatement();

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

	_NODISCARD SQLHSTMT* operator->() noexcept
	{
		return &m_stmt;
	}

private:
	SQLHSTMT m_stmt;
};
} // namespace mssql
} // namespace db
} // namespace core