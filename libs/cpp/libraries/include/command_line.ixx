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

		T value;
		Convert<T>(iter->second, value);
		return value;
	}

protected:
	std::map<std::string, std::string> m_parameter;
};
} // namespace makga::lib