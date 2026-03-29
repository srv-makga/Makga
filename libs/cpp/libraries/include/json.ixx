module;

#include <concepts>
#include <format>
#include <string>
#include <type_traits>

export module makga.lib.json;

export namespace makga::lib {

// ─── JsonSerializable concept ─────────────────────────────────────────────────
// @brief ToJson() -> std::string 멤버 함수를 가진 타입
export template<typename T>
concept JsonSerializable = requires(const T& t)
{
	{ t.ToJson() } -> std::same_as<std::string>;
};

// ─── ToJsonValue ────────────────────────────────────────────────────────────

// @brief 정수형 → JSON number
export template<std::integral T>
	requires (!std::same_as<T, bool>)
std::string ToJsonValue(T v)
{
	return std::to_string(v);
}

// @brief 부동소수점 → JSON number (최단 왕복 표현)
export template<std::floating_point T>
std::string ToJsonValue(T v)
{
	return std::format("{}", v);
}

// @brief bool → JSON boolean
export std::string ToJsonValue(bool v)
{
	return v ? "true" : "false";
}

// @brief std::string → JSON string (이스케이프 처리)
export std::string ToJsonValue(const std::string& v)
{
	std::string result;
	result.reserve(v.size() + 2);
	result += '"';
	for (char c : v)
	{
		switch (c)
		{
			case '"':  result += "\\\""; break;
			case '\\': result += "\\\\"; break;
			case '\n': result += "\\n";  break;
			case '\r': result += "\\r";  break;
			case '\t': result += "\\t";  break;
			default:   result += c;      break;
		}
	}
	result += '"';
	return result;
}

// @brief JsonSerializable 타입 → T::ToJson() 위임 (중첩 컨테이너 재귀 지원)
export template<JsonSerializable T>
std::string ToJsonValue(const T& v)
{
	return v.ToJson();
}
} // namespace makga::lib
