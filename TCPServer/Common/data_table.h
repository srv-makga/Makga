#pragma once

#include "common_header.h"
#include "data_struct.h"
#include <fstream>

class DataTable
{
public:
	using Terrains = std::map<TerrainIdx_t, TerrainInfo>;
	using ItemReinforces = std::map<TableIdx_t, ItemRefineInfo>;

public:
	DataTable() = default;
	~DataTable() = default;

	bool Initialize(String8&& _path);
	virtual bool Load();

public:
	const ItemRefineInfo* ItemRefineInfo(TableIdx_t _idx) const;

public:
	const SystemValueInfo& SystemValue() const { return m_system_default_value; }

private:
	String8 m_path;
	std::fstream m_stream;

	SystemValueInfo m_system_default_value;
	Terrains m_terrain_table;
	ItemReinforces m_item_reinforce;
};