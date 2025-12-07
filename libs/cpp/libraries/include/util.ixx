module;

export module makga.lib.util;

import <string>;
import <vector>;

export namespace makga::lib
{
	std::vector<std::string> Tokenize(const std::string& str, char delimiter);
	bool SplitString(const std::string& str, char delimiter, std::string& first, std::string& second);
} // namespace makga::lib