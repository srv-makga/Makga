module;

#include <string>
#include <map>

export module makga.lib.command_line;

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

				if (false == _text.Split(_T('='), first, second))
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
				if (true == arg.Split(_T(','), first, second))
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
	T Get(const char* _str) const
	{
		auto iter = m_parameter.find(_str);
		if (m_parameter.end() == iter)
		{
			return T();
		}

		if constexpr (std::is_same_v<T, bool>)
		{
			return TYPE_CONVERT.StringToBool(iter->second.c_str());
		}
		if constexpr (std::is_same_v<T, int64_t>)
		{
			return TYPE_CONVERT.StringToInt64(iter->second.c_str());
		}
		if constexpr (std::is_same_v<T, uint64_t>)
		{
			return TYPE_CONVERT.StringToUInt64(iter->second.c_str());
		}
		if constexpr (std::is_unsigned_v<T>)
		{
			return TYPE_CONVERT.StringToUInt32(iter->second.c_str());
		}
		if constexpr (std::is_integral_v<T>)
		{
			return TYPE_CONVERT.StringToInt32(iter->second.c_str());
		}
		if constexpr (std::is_floating_point_v<T>)
		{
			return TYPE_CONVERT.StringToFloat(iter->second.c_str());
		}
		if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>)
		{
			return iter->second;
		}

		return _default;
	}

protected:
	std::map<std::string, std::string> m_parameter;
};
} // namespace makga::lib