#pragma once

#include <ctime>
#include <regex>
#include <time.h>

namespace core {
namespace time {
class DateTime
{
public:
	enum class TimeZone
	{
		GMT,
		Local
	};

public:
	constexpr DateTime(TimeZone _time_zone = TimeZone::Local);
	explicit DateTime(std::time_t time, TimeZone _time_zone = TimeZone::Local);
	DateTime(int year, int month = 1, int day = 1, int hour = 0, int min = 0, int sec = 0, TimeZone _time_zone = TimeZone::Local);
	DateTime(const std::tm& tm, TimeZone _time_zone = TimeZone::Local);
	// @detail _datetime format is "YYYY-MM-DD hh:mm:ss"
	DateTime(const std::string& _datetime, TimeZone _time_zone = TimeZone::Local);
	// @detail _datetime format is "YYYY-MM-DD hh:mm:ss"
	DateTime(std::string&& _datetime, TimeZone _time_zone = TimeZone::Local);
	constexpr DateTime(const DateTime& _rhs);
	~DateTime();

public:
	errno_t TimeToTm();
	void TmToTime();

	bool IsBetween(const DateTime& _start, const DateTime& _end) const;

public:
	// 마지막에 TmToTime()을 호출할 것
	int Year() const;
	DateTime& SetYear(int _year);
	DateTime& AddYear(int _year = 1);
	DateTime& SubYear(int _year = 1);

	int Month() const;
	DateTime& SetMonth(int _month);
	DateTime& AddMonth(int _month = 1);
	DateTime& SubMonth(int _month = 1);

	int Day() const;
	DateTime& SetDay(int _day);
	DateTime& AddDay(int _day = 1);
	DateTime& SubDay(int _day = 1);

	int Hour() const;
	DateTime& SetHour(int _hour);
	DateTime& AddHour(int _hour = 1);
	DateTime& SubHour(int _hour = 1);

	int Min() const;
	DateTime& SetMin(int _min);
	DateTime& AddMin(int _min = 1);
	DateTime& SubMin(int _min = 1);

	int Sec() const;
	DateTime& SetSec(int _sec);
	DateTime& AddSec(int _sec = 1);
	DateTime& SubSec(int _sec = 1);

	DateTime& AddWeek(int _week = 1);
	DateTime& SubWeek(int _week = 1);

	// @detail format is YYYY-MM-DD hh:mm:ss
	void SetDateTime(const std::string& _datetime);

	// @detail format is YYYY-MM-DD
	void SetDate(const std::string& _date);
	// @detail hh:mm:ss
	void SetTime(const std::string& _time);

	std::time_t Time() const;
	const std::tm& Tm() const;
	std::tm TmLocal() const;
	std::tm TmGMT() const;

	std::string String() const;

public:
	// @brief 윤달이 있는 년도인지
	static constexpr bool IsLeapYear(int _year);

	// @brief 년 일자 수
	static constexpr int NumberOfDayYear(int _year);

	// @brief 지금 달의  일자 수
	int NumberOfDayThisMonth();

public:
	operator bool() const;
	operator std::time_t() const;

	DateTime& operator=(const DateTime& _other);
	bool operator==(const DateTime& _other) const;
	bool operator!=(const DateTime& _other) const;
	bool operator<(const DateTime& _other) const;
	bool operator<=(const DateTime& _other) const;
	bool operator>(const DateTime& _other) const;
	bool operator>=(const DateTime& _other) const;
	bool operator<(std::time_t _time) const;

private:
	std::time_t m_time;
	std::tm m_tm;
	TimeZone m_time_zone;
};
} // namespace time
} // namespace core