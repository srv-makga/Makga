#pragma once

#include <vector>
#include <memory>

class ComAcceptor;
class ComConnector;

// @brief ����, Ŭ���� �⺻ Ŭ����
class ComBase
{
	using Connectors = std::vector<std::shared_ptr<ComConnector>>;

public:
	ComBase() = default;
	virtual ~ComBase() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	virtual bool Start();
	virtual void End();

	bool Wait();

public:
	std::shared_ptr<ComConnector> GetConnector(ConnectorType _type);

private:
	Connectors m_connectors;
};