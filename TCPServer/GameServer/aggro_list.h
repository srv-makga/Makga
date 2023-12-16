#pragma once

class Actor;

class AggroList
{
public:
	AggroList(Actor* _owner);
	~AggroList();

public:
	void Initialize();

private:
	Actor* m_owner;
};
