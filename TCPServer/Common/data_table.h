#pragma once

#include "common_header.h"
#include "data_struct.h"
#include <fstream>

class DataTable
{
public:
	using TerrainContainer__t = std::map<TerrainIdx_t, TerrainInfo>;

public:
	DataTable() = default;
	~DataTable() = default;

	bool Initialize(String8&& _path);
	virtual bool Load();

protected:

private:
	String8 m_path;
	std::fstream m_stream;

	SystemValueInfo m_system_default_value;
	TerrainContainer__t m_terrain_table;
};