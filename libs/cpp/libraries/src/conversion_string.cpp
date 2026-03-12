module;

// Win32 API — MultiByteToWideChar / WideCharToMultiByte (UTF-8 ↔ UTF-16)
#include <windows.h>

module makga.lib.conversion_string;

import <string>;

namespace makga::lib {
namespace {

std::wstring ToWString(const std::string& str)
{
	if (true == str.empty())
	{
		return {};
	}

	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (0 >= len)
	{
		return {};
	}

	std::wstring result(len - 1, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), len);
	return result;
}

std::string ToString(const std::wstring& wstr)
{
	if (true == wstr.empty())
	{
	return {};
	}

	int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (0 >= len)
	{
		return {};
	}

	std::string result(len - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result.data(), len, nullptr, nullptr);
	return result;
}
} // namespace

// ─── Constructors ─────────────────────────────────────────────────────────────

ConversionString::ConversionString(const std::string& str)
	: string_(str)
	, wstring_(ToWString(str))
{
}

ConversionString::ConversionString(std::string&& str)
	: string_(std::move(str))
	, wstring_(ToWString(string_))
{
}

ConversionString::ConversionString(const std::wstring& str)
	: string_(ToString(str))
	, wstring_(str)
{
}

ConversionString::ConversionString(std::wstring&& str)
	: string_(ToString(str))
	, wstring_(std::move(str))
{
}

void ConversionString::operator=(const std::string& str)
{
	string_  = str;
	wstring_ = ToWString(str);
}

void ConversionString::operator=(std::string&& str)
{
	string_  = std::move(str);
	wstring_ = ToWString(string_);
}

void ConversionString::operator=(const std::wstring& str)
{
	string_  = ToString(str);
	wstring_ = str;
}

void ConversionString::operator=(std::wstring&& str)
{
	string_  = ToString(str);
	wstring_ = std::move(str);
}

// @brief 비-const 객체에서 호출 시 string_ 반환
const std::string& ConversionString::operator()()
{
	return string_;
}

// @brief const 객체에서 호출 시 wstring_ 반환
const std::wstring& ConversionString::operator()() const
{
	return wstring_;
}

void ConversionString::operator()(std::string& str)
{
	str = string_;
}

void ConversionString::operator()(std::wstring& str) const
{
	str = wstring_;
}

// ─── Conversion operators ─────────────────────────────────────────────────────

ConversionString::operator std::string() const
{
	return string_;
}

ConversionString::operator std::wstring() const
{
	return wstring_;
}

// ─── Getters ──────────────────────────────────────────────────────────────────

const std::string& ConversionString::GetString() const
{
	return string_;
}

const std::wstring& ConversionString::GetWString() const
{
	return wstring_;
}
} // namespace makga::lib