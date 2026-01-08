#pragma once

#include "community.h"

class Party : public Community
{
public:
	Party();
	virtual ~Party();

	bool Initialize();
	void Finalize();
};