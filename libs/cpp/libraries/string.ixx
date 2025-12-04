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
	String(std::string data);
	virtual ~String();

	void Clear();
	std::size_t Size() const;
	std::size_t Length() const;
	bool IsEmpty() const;

	void Upper();
	void Lower();

	std::vector<std::string> Split() const;
};
} // namespace makga::lib