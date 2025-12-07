#include <ctime>
#include <errno.h>

module makga.lib.datetime;
import <string>;

namespace makga::lib {
DateTime::DateTime()
	: time_(std::time(nullptr))
	, tm_({0, })
{
	TimeToTM();
}

DateTime::DateTime(std::time_t time)
	: time_(time)
{
	TimeToTM();
}

int DateTime::Year() const
{
	return tm_.tm_year + 1900;
}

DateTime& DateTime::SetYear(int year)
{
	tm_.tm_year = year - 1900;
	return *this;
}

DateTime& DateTime::AddYear(int year)
{
	tm_.tm_year += year;
	return *this;
}

DateTime& DateTime::SubYear(int year)
{
	tm_.tm_year -= year;
	return *this;
}

int DateTime::Month() const
{
	return tm_.tm_mon + 1;
}

DateTime& DateTime::SetMonth(int month)
{
	tm_.tm_mon = month - 1;
	return *this;
}

DateTime& DateTime::AddMonth(int month)
{
	tm_.tm_mon += month;
	return *this;
}

DateTime& DateTime::SubMonth(int month)
{
	tm_.tm_mon -= month;
	return *this;
}

int DateTime::Day() const
{
	return tm_.tm_mday;
}

DateTime& DateTime::SetDay(int day)
{
	tm_.tm_mday = day;
	return *this;
}

DateTime& DateTime::AddDay(int day)
{
	tm_.tm_mday += day;
	return *this;
}

DateTime& DateTime::SubDay(int day)
{
	tm_.tm_mday -= day;
	return *this;
}

int DateTime::Hour() const
{
	return tm_.tm_hour;
}

DateTime& DateTime::SetHour(int hour)
{
	tm_.tm_hour = hour;
	return *this;
}

DateTime& DateTime::AddHour(int hour)
{
	tm_.tm_hour += hour;
	return *this;
}

DateTime& DateTime::SubHour(int hour)
{
	tm_.tm_hour -= hour;
	return *this;
}

int DateTime::Min() const
{
	return tm_.tm_min;
}

DateTime& DateTime::SetMin(int min)
{
	tm_.tm_min = min;
	return *this;
}

DateTime& DateTime::AddMin(int min)
{
	tm_.tm_min += min;
	return *this;
}

DateTime& DateTime::SubMin(int min)
{
	tm_.tm_min -= min;
	return *this;
}

int DateTime::Sec() const
{
	return tm_.tm_sec;
}

DateTime& DateTime::SetSec(int sec)
{
	tm_.tm_sec = sec;
	return *this;
}

DateTime& DateTime::AddSec(int sec)
{
	tm_.tm_sec += sec;
	return *this;
}

DateTime& DateTime::SubSec(int sec)
{
	tm_.tm_sec -= sec;
	return *this;
}

std::string DateTime::String(const char* format) const
{
	thread_local char buffer[64] = { 0, };
	std::strftime(buffer, sizeof(buffer), format, &tm_);
	return std::string(buffer);
}

DateTime::operator bool() const
{
	return 0 < time_;
}

DateTime::operator std::time_t() const
{
	return time_;
}

bool DateTime::operator<(std::time_t time) const
{
	return time_ < time;
}

void DateTime::TimeToTM()
{
	errno_t err = ::localtime_s(&tm_, &time_);
}
} // namespace makga::lib