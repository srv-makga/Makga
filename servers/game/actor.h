#pragma once

class Actor
{
public:
	Actor();
	virtual ~Actor();

	bool Initialize();
	void Finalize();
};
