module;

export module makga.lib.algorithm.ahocorasick;

import <vector>;
import <string>;
import <unordered_map>;

export namespace makga::lib {
class TrieNode
{
public:
	TrieNode();
	void InsertString(const std::string& str, std::size_t index);
	void ConstructFailureLinks();
	bool IsMatch(const std::string& text);
	const std::string& GetMatchString(const std::string& text);

protected:
	std::size_t Search(const std::string& _text);

private:
	std::unordered_map<char, TrieNode*> childrens_;
	TrieNode* failure_link_;
	bool is_end_word_;
	std::size_t index_;
	std::vector<std::string> texts_;
};
} // namespace makga::lib