module makga.lib.util;

import <string>;
import <vector>;

namespace makga::lib {
std::vector<std::string> Tokenize(const std::string& str, char delimiter)
{
	std::vector<std::string> result;

	do
	{
		if (true == str.empty())
		{
			break;
		}

		std::string::size_type start = 0;
		std::string::size_type end = str.find(delimiter);
		while (std::string::npos != end)
		{
			result.push_back(str.substr(start, end - start));

			start = end + 1;
			end = str.find(delimiter, start);
		}

		result.push_back(str.substr(start));
	} while (false);

	return result;
}

bool SplitString(const std::string& str, char delimiter, std::string& first, std::string& second)
{
	std::string::size_type pos = str.find(delimiter);
	if (std::string::npos == pos)
	{
		return false;
	}

	first = str.substr(0, pos);
	second = str.substr(pos + 1);
	return true;
}
} // namespace makga::lib