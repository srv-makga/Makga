#pragma once

#include <queue>
#include <unordered_map>
#include <vector>
#include <string>

namespace core {
namespace algorithm {
// @detail �ؽ�Ʈ �˻��� root�� �� Node�� �ش� node�� ��ӹް�, insert�Ǵ� string ����� ������ ������ ��Ī�� �ؽ�Ʈ ��ȯ�� ����
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