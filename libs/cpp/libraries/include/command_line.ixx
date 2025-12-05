module;

#include <string>
#include <map>

export module makga.lib.command_line;

import makga.lib.convert;
import makga.lib.util;

export namespace makga::lib {
export class CommandLine
{
public:
	CommandLine(int argc, char* argv[])
	{
		static const auto lambda = [this](std::string& _text)
			{
				std::string first;
				std::string second;

				if (false == SplitString(_text, '=', first, second))
				{
					return;
				}

				m_parameter.insert({ first, second });
			};

		std::string first;
		std::string second;

		for (int i = 1; i < argc; ++i)
		{
			std::string arg(argv[i]);

			do
			{
				if (true == SplitString(arg, ',', first, second))
				{
					lambda(first);
					arg = second;
				}
			} while (false);

			lambda(arg);
		}
	}
	virtual ~CommandLine() = default;

	template<typename T>
	T Get(const char* str) const
	{
		auto iter = m_parameter.find(str);
		if (m_parameter.end() == iter)
		{
			return T();
		}

		if constexpr (std::is_same_v<T, bool>)
		{
			return StringToBool(iter->second.c_str());
		}
		if constexpr (std::is_same_v<T, int64_t>)
		{
			return StringToInt64(iter->second.c_str());
		}
		if constexpr (std::is_same_v<T, uint64_t>)
		{
			return StringToUInt64(iter->second.c_str());
		}
		if constexpr (std::is_unsigned_v<T>)
		{
			return StringToUInt32(iter->second.c_str());
		}
		if constexpr (std::is_integral_v<T>)
		{
			return StringToInt32(iter->second.c_str());
		}
		if constexpr (std::is_floating_point_v<T>)
		{
			return StringToFloat(iter->second.c_str());
		}
		if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>)
		{
			return iter->second;
		}

		return T();
	}

protected:
	std::map<std::string, std::string> m_parameter;
};
} // namespace makga::lib