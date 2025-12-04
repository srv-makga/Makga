import makga.lib.command_line

namespace makga::lib {
CommandLine::CommandLine(int argc, char* argv[])
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
} // namespace makga::lib