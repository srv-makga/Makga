#include <ctime>
#include <regex>
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

DateTime& DateTime::SetDateTime(const std::string& datetime)
{
	static const std::regex reg("(\\d{4})-(\\d{1,2})-(\\d{1,2}) (\\d{1,2}):(\\d{1,2}):(\\d{1,2})");
	std::smatch match;

	if (std::regex_match(datetime, match, reg) && 7 == match.size())
	{
		tm_.tm_year = stoi(*(match.begin() + 1)) - 1900;
		tm_.tm_mon = stoi(*(match.begin() + 2)) - 1;
		tm_.tm_mday = stoi(*(match.begin() + 3));

		tm_.tm_hour = stoi(*(match.begin() + 4));
		tm_.tm_min = stoi(*(match.begin() + 5));
		tm_.tm_sec = stoi(*(match.begin() + 6));
	}

	return *this;
}

DateTime& DateTime::SetDate(const std::string& _date)
{
	static const std::regex reg("(\\d{4})-(\\d{1,2})-(\\d{1,2})");
	std::smatch match;

	if (std::regex_match(_date, match, reg) && 4 == match.size())
	{
		tm_.tm_year = stoi(*(match.begin() + 1)) - 1900;
		tm_.tm_mon = stoi(*(match.begin() + 2)) - 1;
		tm_.tm_mday = stoi(*(match.begin() + 3));
	}

	return *this;
}

DateTime& DateTime::SetTime(const std::string& time)
{
	static const std::regex reg("(\\d{1,2}):(\\d{1,2}):(\\d{1,2})");
	std::smatch match;

	if (std::regex_match(time, match, reg) && 4 == match.size())
	{
		tm_.tm_hour = std::stoi(*(match.begin() + 1));
		tm_.tm_min = std::stoi(*(match.begin() + 2));
		tm_.tm_sec = std::stoi(*(match.begin() + 3));
	}

	return *this;
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

std::time_t DateTime::CalculateTime()
{
	TMToTime();
	return Time();
}

std::string DateTime::String(const char* format) const
{
	char buffer[64] = { 0, };
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

DateTime& DateTime::operator=(const DateTime& other)
{
	time_ = other.time_;
	::memcpy(&tm_, &other.tm_, sizeof(tm_));
	return *this;
}

bool DateTime::operator==(const DateTime& other) const
{
	return time_ == other.time_;
}

bool DateTime::operator!=(const DateTime& other) const
{
	return time_ != other.time_;
}

bool DateTime::operator<(const DateTime& other) const
{
	return time_ < other.time_;
}

bool DateTime::operator<=(const DateTime& other) const
{
	return time_ <= other.time_;
}

bool DateTime::operator>(const DateTime& other) const
{
	return time_ > other.time_;
}

bool DateTime::operator>=(const DateTime& other) const
{
	return time_ >= other.time_;
}

bool DateTime::operator==(std::time_t time) const
{
	return time_ == time;
}

bool DateTime::operator<(std::time_t time) const
{
	return time_ < time;
}

bool DateTime::operator>(std::time_t time) const
{
	return time_ > time;
}

void DateTime::TimeToTM()
{
	errno_t err = ::localtime_s(&tm_, &time_);
}

void DateTime::TMToTime()
{
	time_ = std::mktime(&tm_);
}
} // namespace makga::lib