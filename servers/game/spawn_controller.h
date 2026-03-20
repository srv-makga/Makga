#pragma once

// ���� ������ ������ �ִ´�.
// ������ �������� ���� �޴´�
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
