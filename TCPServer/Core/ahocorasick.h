#pragma once

#include <queue>
#include <unordered_map>
#include <vector>
#include <string>

namespace core {
namespace algorithm {
// @detail 텍스트 검색의 root가 될 Node는 해당 node를 상속받고, insert되는 string 목록을 가지고 있으면 매칭된 텍스트 반환도 가능
class TrieNode
{
public:
	TrieNode();
	void InsertString(const std::string& _str, std::size_t _index);
	void ConstructFailureLinks();
	bool IsMatch(const std::string& _text);
	const std::string& GetMatchString(const std::string& _text);

protected:
	std::size_t Search(const std::string& _text);

private:
	std::unordered_map<char, TrieNode*> m_childrens;
	TrieNode* m_failure_link;
	bool m_is_end_word;
	std::size_t m_index;
	std::vector<std::string> m_texts;
};
} // namespace algorithm
} // namespace core