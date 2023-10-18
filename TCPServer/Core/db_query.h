#pragma once

#include <string>

namespace core {
namespace db {
class DBQuery
{
public:
	virtual bool Prepare(const std::string& query) = 0;
	virtual bool Bind(int index, int value) = 0;
	virtual bool Bind(int index, const std::string& value) = 0;
	virtual bool Execute() = 0;
	virtual bool FetchRow() = 0;
	virtual int ColumnCount() const = 0;
	virtual std::string ColumnValue(int index) const = 0;
};
} // namespace db
} // namespace core
