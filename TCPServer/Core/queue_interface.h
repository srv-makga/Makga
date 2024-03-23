#pragma once

namespace core {
namespace queue {
class QueueInterface
{
public:
	QueueInterface() = default;
	virtual ~QueueInterface() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;

	// @todo Push, Pop�� ���� ���� �Լ��� �ۼ��ص���
};
} // namespace queue
} // namespace core