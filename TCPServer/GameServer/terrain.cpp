#include "pch.h"
#include "terrain.h"
#include "terrain_grid_manager.h"
#include "terrain_grid.h"

Terrain::Terrain(TerrainUid_t _uid)
	: m_uid(_uid)
	, m_table(nullptr)
	, m_nav_mesh(nullptr)
	, m_nav_query(nullptr)
	, m_grid_manager(nullptr)
{
}

Terrain::~Terrain()
{
	dtFreeNavMeshQuery(m_nav_query);
	dtFreeNavMesh(m_nav_mesh);
}

bool Terrain::Initialize(const TerrainInfo& _table)
{
	m_table = &_table;

	dtFreeNavMeshQuery(m_nav_query);
	m_nav_mesh = dtAllocNavMesh();

	dtFreeNavMesh(m_nav_mesh);
	m_nav_query = dtAllocNavMeshQuery();

	if (nullptr == m_nav_mesh || nullptr == m_nav_query)
	{
		LOG_ERROR << "m_nav_mesh or m_nav_query is nullptr";
		return false;
	}

	if (false == LoadNavMesh(_table.nav_mesh_name))
	{
		LOG_ERROR << "LoadNavMesh Fail. nav_mesh_name:" << _table.nav_mesh_name.c_str();
		return false;
	}

	m_nav_query->init(m_nav_mesh, 2048);
	//m_nav_filter.setAreaCost(PolyAreas::SAMPLE_POLYAREA_GROUND, 1.0f);
	//m_nav_filter.setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
	//m_nav_filter.setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
	//m_nav_filter.setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
	//m_nav_filter.setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
	//m_nav_filter.setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);

	delete m_grid_manager;
	m_grid_manager = new TerrainGridManager();
	m_grid_manager->Initialize(_table);

	// @todo
	// m_instance = nullptr;

	return true;
}

void Terrain::Finalize()
{
}

void Terrain::OnUpdate()
{
	m_grid_manager->OnUpdate();
}

Result_t Terrain::CanMove(const PositionT& _position)
{
	// 내비메쉬 체크
	CheckNaviMesh(_position.x, _position.y, _position.z);

	// 다른 액터 충돌 체크
}

bool Terrain::IsInside(const Vector_t& _vector)
{
	if (m_table->left_top.first < _vector.X())
	{
		return false;
	}

	if (m_table->left_top.second < _vector.Y())
	{
		return false;
	}

	if (m_table->right_bottom.first > _vector.X())
	{
		return false;
	}

	if (m_table->right_bottom.second > _vector.Y())
	{
		return false;
	}

	return true;
}

TerrainGrid* Terrain::FindGrid(Coord_t _x, Coord_t _y, Coord_t _z)
{
	return m_grid_manager->FindGrid(_x, _y, _z);
}

void Terrain::FindNotificationList(const PositionT& _old, const PositionT& _new, Count_t _max_count, OUT UserList& _appear_list, OUT UserList& _disappear_list, OUT UserList& _move_list)
{
	// 나 밖에 없다면야
	if (0 == CurUserCount())
	{
		return;
	}

	TerrainGrid* old_grid = FindGrid(_old.x, _old.y, _old.z);
	if (nullptr == old_grid)
	{
		return;
	}

	TerrainGrid* new_grid = FindGrid(_new.x, _new.y, _new.z);
	if (nullptr == new_grid)
	{
		return;
	}

	std::vector<TerrainGrid*> appear_grid_list;
	std::vector<TerrainGrid*> disappear_grid_list;
	std::vector<TerrainGrid*> move_grid_list;

	// 우선 9x9에서만 찾는다

	if (old_grid == new_grid)
	{
		appear_grid_list.push_back(old_grid);
		disappear_grid_list.push_back(old_grid);
		move_grid_list.push_back(old_grid);
	}
	else
	{
		Coord_t change_x = _old.x - _new.x;
		Coord_t change_y = _old.y - _new.y;
		Coord_t change_z = _old.z - _new.z;

		// 좌표 변화에 따른 주변 grid 가져오기
		std::vector<TerrainGrid*> grid_list;
		old_grid->GridListByChangeCoord(_old, _new, grid_list);
		
		if (true == grid_list.empty())
		{
			return;
		}

		for (TerrainGrid* grid : grid_list)
		{
			old_grid->GridListByChangeCoord
			// disappear - 기존 좌표에서 일정거리 내에 있고 신규 위치 일정거리 내에 없는
			// appear  - 기존 좌표에서 일정거리 내에 없고 신규 위치 일정거리 내에 있는
			// move - 기존 좌표에서 일정거리 내에 있고 신규 위치 일정거리 내에 없는
		}
	}
}

bool Terrain::AroundList(const PositionT& _position, Distance_t _range, OUT ActorList& _actor_list)
{
	// @todo 구현
	return !_actor_list.empty();
}

Count_t Terrain::CurUserCount() const
{
	return m_actor_list.size();
}

TerrainIdx_t Terrain::Idx() const
{
	return nullptr == m_table
		? 0
		: m_table->idx;
}

bool Terrain::LoadNavMesh(const String8& _file_name)
{
	FILE* fp = fopen(_file_name.c_str(), "rb");
	if (!fp)
	{
		return 0;
	}

	static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
	static const int NAVMESHSET_VERSION = 1;

	// Read header.
	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1)
	{
		fclose(fp);
		return 0;
	}
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return 0;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return 0;
	}

	dtStatus status = m_nav_mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return 0;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			return 0;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			return 0;
		}
		m_nav_mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(fp);

	return true;
}
