module;

#include <ctime>
import <string>;

export module makga.lib.datetime;

export namespace makga::lib {
class DateTime
{
public:
	DateTime();
	DateTime(std::time_t time);
	virtual ~DateTime() = default;

	int Year() const;
	DateTime& SetYear(int year);
	DateTime& AddYear(int year = 1);
	DateTime& SubYear(int year = 1);

	int Month() const;
	DateTime& SetMonth(int month);
	DateTime& AddMonth(int month = 1);
	DateTime& SubMonth(int month = 1);

	int Day() const;
	DateTime& SetDay(int day);
	DateTime& AddDay(int day = 1);
	DateTime& SubDay(int day = 1);

	int Hour() const;
	DateTime& SetHour(int hour);
	DateTime& AddHour(int hour = 1);
	DateTime& SubHour(int hour = 1);

	int Min() const;
	DateTime& SetMin(int min);
	DateTime& AddMin(int min = 1);
	DateTime& SubMin(int min = 1);

	int Sec() const;
	DateTime& SetSec(int sec);
	DateTime& AddSec(int sec = 1);
	DateTime& SubSec(int sec = 1);

	std::string String(const char* foramt = "%Y-%m-%d %H:%M:%S") const;

	operator bool() const;
	operator std::time_t() const;
	bool operator<(std::time_t time) const;

	std::time_t Time() const { return time_; }
	const std::tm& TM() const { return tm_; }

private:
	void TimeToTM();

private:
	std::time_t time_;
	std::tm tm_;
};
} // namespace makga::lib