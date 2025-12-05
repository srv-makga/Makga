module makga.lib.algorithm.ahocorasick;

import <queue>;

namespace makga::lib {
TrieNode::TrieNode()
	: failure_link_(nullptr)
	, is_end_word_(false)
	, index_(-1)
{
}

void TrieNode::InsertString(const std::string& str, std::size_t index)
{
	TrieNode* current = this;

	for (char c : str)
	{
		if (current->childrens_.end() == current->childrens_.find(c))
		{
			current->childrens_.insert({ c, new TrieNode });
		}

		current = current->childrens_[c];
	}

	current->is_end_word_ = true;
	current->index_ = index;

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

	while (!q.empty())
	{
		TrieNode* current = q.front();
		q.pop();

		for (auto [c, children] : current->childrens_)
		{
			TrieNode* failure = current->failure_link_;

			while (nullptr != failure && failure->childrens_.find(c) == failure->childrens_.end())
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

std::size_t TrieNode::Search(const std::string& _text)
{
	TrieNode* current = this;

	for (char c : _text)
	{
		while (nullptr != current && current->childrens_.end() != current->childrens_.find(c))
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