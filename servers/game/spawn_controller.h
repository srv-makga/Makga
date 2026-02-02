#pragma once

// 스폰 정보를 가지고 있는다.
// 액터의 소유권을 이전 받는다
class SpawnController
{
public :
	SpawnController();
	virtual ~SpawnController();

	bool Initialize();
	void Finalize();
	bool Load(const std::vector<TableIdx>& spawn_group_ids);

	bool StartAllSpawn();
	void StopAllSpawn();

	bool StartGroupSpawn(TableIdx spawn_group_id);
	bool StopGroupRespawn(TableIdx spawn_group_id);

protected:
};