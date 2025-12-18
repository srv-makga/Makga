#include <queue>

module makga.lib.algorithm.ahocorasick;

import <vector>;
import <string>;
import <unordered_map>;

namespace makga::lib {
TrieNode::TrieNode()
	: failure_link_(nullptr)
	, is_end_word_(false)
	, index_(-1)
{
}

void TrieNode::InsertString(const std::string& str)
{
	TrieNode* current = this;

	for (char c : str)
	{
		auto iter = current->childrens_.find(c);
		if (current->childrens_.end() == iter)
		{
			iter = current->childrens_.insert({ c, new TrieNode }).first;
		}

		current = iter->second;
	}

	current->is_end_word_ = true;
	current->index_ = texts_.size();

	texts_.push_back(str);
}

void TrieNode::ConstructFailureLinks()
{
	std::queue<TrieNode*> q;

	for (auto& [c, children] : childrens_)
	{
		children->failure_link_ = this;
		q.push(children);
	}

	while (false == q.empty())
	{
		TrieNode* current = q.front();
		q.pop();

		for (auto [c, children] : current->childrens_)
		{
			TrieNode* failure = current->failure_link_;

			while (nullptr != failure && failure->childrens_.end() == failure->childrens_.find(c))
			{
				failure = failure->failure_link_;
			}

			if (nullptr == failure)
			{
				children->failure_link_ = this;
			}
			else
			{
				children->failure_link_ = failure->childrens_[c];
			}

			q.push(children);
		}
	}
}

bool TrieNode::IsMatch(const std::string& text)
{
	return -1 != Search(text);
}

const std::string& TrieNode::GetMatchString(const std::string& text)
{
	std::size_t pos = Search(text);
	if (-1 == pos)
	{
		static const std::string s_empty;
		return s_empty;
	}

	return texts_[pos];
}

std::size_t TrieNode::Search(const std::string& text)
{
	TrieNode* current = this;

	for (char c : text)
	{
		while (nullptr != current && current->childrens_.end() == current->childrens_.find(c))
		{
			current = current->failure_link_;
		}

		if (nullptr == current)
		{
			current = this;
		}
		else
		{
			current = current->childrens_[c];

			if (true == current->is_end_word_)
			{
				return current->index_;
			}
		}
	}

	return -1;
}
} // namespace makga::lib