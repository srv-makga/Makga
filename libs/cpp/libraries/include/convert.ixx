module;

#include <string>

export module makga.lib.convert;

export namespace makga::lib
{
	int32_t StringToInt32(const char* _value)
	{
		return std::stoi(_value);
	}

	uint32_t StringToUInt32(const char* _value)
	{
		return static_cast<uint32_t>(std::stoul(_value));
	}

	int64_t StringToInt64(const char* _value)
	{
		return static_cast<int64_t>(std::stoll(_value));
	}

	uint64_t StringToUInt64(const char* _value)
	{
		return static_cast<uint64_t>(std::stoull(_value));
	}

	float StringToFloat(const char* _value)
	{
		return std::stof(_value);
	}

	double StringToDouble(const char* _value)
	{
		return std::stod(_value);
	}

	bool StringToBool(const char* _value)
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
} // namespace makga::lib