#pragma once

#include "string_function.h"
#include <algorithm>

namespace core {
namespace string {
template <typename T>
inline constexpr bool is_character_v = std::_Is_any_of_v<std::remove_cv_t<T>, char, wchar_t
#ifdef __cpp_char8_t
, char8_t, char16_t, char32_t
#endif // __cpp_char8_t
>;

template <typename T>
concept StringElement_t = is_character_v<T>;

template<StringElement_t T>
class String
{
public:
	using String_t = std::basic_string<T, std::char_traits<T>, std::allocator<T>>;

public:
	String()
	{
	}

	String(const T* _rhs)
		: m_str(_rhs)
	{
	}

	String(const String_t& _rhs)
		: m_str(_rhs)
	{
	}

	String(String_t&& _rhs)
		:m_str(std::move(_rhs))
	{
	}

	~String()
	{
		m_str.clear();
	}

	void Initialize()
	{
		m_str.clear();
	}

	std::size_t Size() const
	{
		return m_str.size();
	}

	bool Empty() const
	{
		return m_str.empty();
	}

	void Clear()
	{
		m_str.clear();
	}

	void Replace(const String_t& _before, const String_t& _after)
	{
		//std::size_t pos = 0;
		//while (Type::npos != (pos = m_str.find(before, pos)))
		//{
		//	m_str.replace(pos, before.length(), after);
		//	pos = after.length();
		//}

		m_str = std::regex_replace(m_str, std::regex(_before), _after);
	}

	void Replace(const T* _before, const T* _after)
	{
		//std::size_t pos = 0;
		//while (Type::npos != (pos = m_str.find(before, pos)))
		//{
		//	m_str.replace(pos, before.length(), after);
		//	pos = after.length();
		//}

		m_str = std::regex_replace(m_str, std::regex(_before), _after);
	}

	// @brief 대문자로 변경
	void Upper()
	{
		std::transform(m_str.begin(), m_str.end(), m_str.begin(), std::toupper);
	}

	// @brief 소문자로 변경
	void Lower()
	{
		std::transform(m_str.begin(), m_str.end(), m_str.begin(), std::tolower);
	}

	// 우측 여백 제거
	void Rtrim()
	{
		m_str.erase(m_str.find_last_not_of(_T("\t\r\n\f\v)") + 1));
	}

	bool Split(T _delim, String<T>& _first, String<T>& _second)
	{
		std::size_t pos = m_str.find(_delim);
		if (String_t::npos == pos)
		{
			return false;
		}

		_first = std::move(m_str.substr(0, pos));
		_second = std::move(m_str.substr(pos + 1));
		return true;
	}

	String<T>& operator= (const String<T>& _rhs)
	{
		m_str = _rhs.m_str;
		return *this;
	}

	String<T>& operator= (String<T>&& _rhs)
	{
		m_str = std::move(_rhs.m_str);
		return *this;
	}

	operator String_t() const
	{
		return m_str;
	}

	operator const T* ()
	{
		return m_str.c_str();
	}

private:
	String_t m_str;
};
} // namespace string
} // namespace core