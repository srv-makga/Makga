module;

#include <string>
#include <vector>

export module makga.lib.string;

export namespace makga::lib
{
class String
{
public:
	String();
	virtual ~String();

	void Clear();
	std::size_t Size() const;
	std::size_t Length() const;
	bool IsEmpty() const;

	std::vector<std::string> Split() const;
};
} // namespace makga::lib