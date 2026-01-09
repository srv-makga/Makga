module;

#include <string>
#include <cwchar>
#include <stdexcept>
#include <windows.h>

export module makga.lib.convert;

export namespace makga::lib
{
	export template<typename T>
	bool Convert(const std::string& value, OUT T& result)
	{
		return false;
	}

	export template<>
	bool Convert(const std::string& value, OUT int32_t& result)
	{
		std::size_t end = 0;
		result = std::stoi(value, &end);
		return end == value.size();
	}

	export template<>
	bool Convert(const std::string& value, OUT long& result)
	{
		std::size_t end = 0;
		result = std::stol(value, &end);
		return end == value.size();
	}

	export template<>
	bool Convert(const std::string& value, OUT unsigned long& result)
	{
		std::size_t end = 0;
		result = std::stoul(value, &end);
		return end == value.size();
	}

	export template<>
	bool Convert(const std::string& value, OUT float& result)
	{
		std::size_t end = 0;
		result = std::stof(value, &end);
		return end == value.size();
	}

	export template<>
	bool Convert(const std::string& value, OUT double& result)
	{
		std::size_t end = 0;
		result = std::stod(value, &end);
		return end == value.size();
	}

	export template<>
	bool Convert(const std::string& value, OUT bool& result)
	{
		if (value.size() != 4 && value.size() != 5)
		{
			return false;
		}

		std::string temp = value;
		for (char& c : temp)
		{
			c = static_cast<char>(std::tolower(c));
		}

		if (temp == "true")
		{
			result = true;
			return true;
		}
		else if (temp == "false")
		{
			result = false;
			return true;
		}

		return false;
	}

	export template<>
	bool Convert(const std::string& value, OUT int64_t& result)
	{
		std::size_t end = 0;
		result = std::stoll(value, &end);
		return end == value.size();
	}

	export template<>
	bool Convert(const std::string& value, OUT uint64_t& result)
	{
		std::size_t end = 0;
		result = std::stoull(value, &end);
		return end == value.size();
	}

	bool Convert(const std::string& value, OUT std::wstring& result)
	{
		if (true == value.empty())
		{
			result.clear();
			return true;
		}

		int required = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.c_str(), -1, nullptr, 0);
		if (0 == required)
		{
			return false;
		}

		result.resize(required);

		int converted = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.c_str(), -1, &result[0], required);
		if (0 == converted)
		{
			return false;
		}

		result.resize(converted - 1);
		return true;
	}

	export bool Convert(const std::wstring& str, OUT std::string& result)
	{
		if (true == str.empty())
		{
			result.clear();
			return true;
		}

		int required = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (0 == required)
		{
			return false;
		}

		result.resize(required);

		int converted = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.c_str(), -1, &result[0], required, nullptr, nullptr);
		if (0 == converted)
		{
			return false;
		}

		result.resize(converted - 1);
		return true;
	}
} // namespace makga::lib