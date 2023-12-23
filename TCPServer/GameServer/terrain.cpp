#include "pch.h"
#include "terrain.h"
#include "terrain_grid_manager.h"
#include "terrain_grid.h"

Terrain::Terrain(TerrainUid_t _uid)
	: m_uid(_uid)
	, m_table(nullptr)
	, m_nav_mesh(nullptr)
	, m_nav_query(nullptr)
	, m_nav_filter(nullptr)
	, m_grid_manager(nullptr)
{
}

Terrain::~Terrain()
{
	delete m_nav_filter;
	dtFreeNavMeshQuery(m_nav_query);
	dtFreeNavMesh(m_nav_mesh);
}

bool Terrain::Initialize(const TerrainInfo& _table)
{
	m_table = &_table;

	delete m_nav_filter;
	m_nav_filter = new dtQueryFilter();
	if (nullptr == m_nav_filter)
	{
		LOG_ERROR << "m_nav_filter is nullptr";
		return false;
	}

	dtFreeNavMeshQuery(m_nav_query);
	m_nav_mesh = dtAllocNavMesh();

	dtFreeNavMesh(m_nav_mesh);
	m_nav_query = dtAllocNavMeshQuery();

	if (nullptr == m_nav_mesh || nullptr == m_nav_query)
	{
		dtFreeNavMeshQuery(m_nav_query);
		dtFreeNavMesh(m_nav_mesh);

		LOG_ERROR << "m_nav_mesh or m_nav_query is nullptr";
		return false;
	}

	if (false == LoadNavMesh(_table.nav_mesh_name))
	{
		LOG_ERROR << "LoadNavMesh Fail. nav_mesh_name:" << _table.nav_mesh_name.c_str();
	}

	delete m_grid_manager;
	m_grid_manager = new TerrainGridManager();
	m_grid_manager->Initialize(_table);

	return true;
}

void Terrain::Finalize()
{
}

void Terrain::OnUpdate()
{
	m_grid_manager->OnUpdate();
}

Result_t Terrain::DoMove(Actor* _actor, const PositionT& _position)
{
	if (nullptr == _actor)
	{
		return eResult_ActorNotFound;
	}



	return Result_t();
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

bool Terrain::AroundList(const PositionT& _position, Distance_t _range, OUT ActorList& _actor_list)
{
	// @todo ±¸Çö
	return !_actor_list.empty();
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
