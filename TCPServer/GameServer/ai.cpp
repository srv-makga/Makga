#include "pch.h"
#include "ai.h"
#include "actor.h"

AI::AI(std::shared_ptr<Actor> _owner)
	: m_owner(_owner)
	, m_root(nullptr)
{
}

AI::~AI()
{
	if (m_root)
	{
		m_root->terminate(Status::Failure);
	}

	m_owner = nullptr;
	m_root = nullptr;
}

void AI::terminate(Status s)
{
	if (nullptr != m_owner)
	{
		// @todo ¸ó½ºÅÍ ¾÷µ¥ÀÌÆ® ¸ØÃã?
	}

	if (nullptr != m_root)
	{
		m_root->terminate(s);
	}
}

std::shared_ptr<Actor> AI::Owner() const
{
	return m_owner;
}

AI::Node::Ptr AI::Root() const
{
	return m_root;
}
