#include <algorithm>
#include <string>

module makga.lib.string;

import <vector>;
import makga.lib.util;

namespace makga::lib {
String::String(const std::string& data)
	: data_(data)
{
}

String::String(std::string&& data)
	: data_(std::move(data))
{
}

void String::Clear()
{
	data_.clear();
}

std::size_t String::Size() const
{
	return data_.size();
}

std::size_t String::Length() const
{
	return data_.length();
}

bool String::IsEmpty() const
{
	return data_.empty();
}

std::string String::Upper() const
{
	if (true == data_.empty())
	{
		return std::string();
	}

	std::string result;
	std::transform(data_.begin(), data_.end(), result.begin(),
		[](unsigned char ch) -> char { return static_cast<char>(std::toupper(ch)); });

	return result;
}

std::string String::Lower() const
{
	if (true == data_.empty())
	{
		return std::string();
	}

	std::string result;
	std::transform(data_.begin(), data_.end(), result.begin(),
		[](unsigned char ch) -> char { return static_cast<char>(std::tolower(ch)); });

	return result;
}

std::vector<std::string> String::Tokenize(char delimiter) const
{
	return makga::lib::Tokenize(data_, delimiter);
}
} // namespace makga::lib