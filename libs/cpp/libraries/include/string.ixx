module;

export module makga.lib.string;

import <string>;
import <vector>;

export namespace makga::lib
{
class String
{
public:
	String() = default;
	String(const std::string& data);
	String(std::string&& data);
	virtual ~String() = default;

	void Clear();
	std::size_t Size() const;
	std::size_t Length() const;
	bool IsEmpty() const;

	std::string Upper();
	std::string Lower();

	std::vector<std::string> Tokenize(char delimiter) const;

private:
	std::string data_;
};
} // namespace makga::lib