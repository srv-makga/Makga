module;

#include <string>
#include <cwchar>
#include <stdexcept>
#include <windows.h>

export module makga.lib.convert;

export namespace makga::lib
{
	export int32_t StringToInt32(const char* _value)
	{
		return std::stoi(_value);
	}

	export uint32_t StringToUInt32(const char* _value)
	{
		return static_cast<uint32_t>(std::stoul(_value));
	}

	export int64_t StringToInt64(const char* _value)
	{
		return static_cast<int64_t>(std::stoll(_value));
	}

	export uint64_t StringToUInt64(const char* _value)
	{
		return static_cast<uint64_t>(std::stoull(_value));
	}

	export float StringToFloat(const char* _value)
	{
		return std::stof(_value);
	}

	export double StringToDouble(const char* _value)
	{
		return std::stod(_value);
	}

	export bool StringToBool(const char* _value)
	{
		static const std::string s_true_str = "true";
		static const std::size_t s_true_str_size = s_true_str.size();

		std::string temp(_value);

		if (s_true_str_size != temp.size())
		{
			return false;
		}

		for (std::size_t i = 0; i < s_true_str_size; ++i)
		{
			if (s_true_str[i] != std::tolower(temp[i]))
			{
				return false;
			}
		}

		return true;
	}

	export std::wstring StringToWString(const std::string& str)
	{
		if (true == str.empty())
		{
			return std::wstring();
		}

		// Get required size (includes terminating null)
		int required = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
		if (0 == required)
		{
			return std::wstring();
		}

		std::wstring result(required, L'\0');

		int converted = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, &result[0], required);
		if (0 == converted)
		{
			return std::wstring();
		}

		result.resize(converted - 1);
		return result;
	}

	export std::string WStringToString(const std::wstring& wstr)
	{
		if (true == wstr.empty())
		{
			return std::string();
		}

		int required = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (0 == required)
		{
			return std::string();
		}

		std::string result(required, '\0');

		int converted = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.c_str(), -1, &result[0], required, nullptr, nullptr);
		if (0 == converted)
		{
			return std::string();
		}

		result.resize(converted - 1);
		return result;
	}
} // namespace makga::lib