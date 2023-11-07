#pragma once

#include "../Core/singleton.hpp"

class ItemObjectBase;

class ReinforceManager : public core::pattern::Singleton<ReinforceManager>
{
public:
	ReinforceManager() = default;
	~ReinforceManager() = default;

	void Initialize();

	Result_t Run(ItemObjectBase* _main, ItemObjectBase* _materials);

protected:
};

#define REINFORCE_MANAGER ReinforceManager::Instance()