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

	// @todo Push, Pop에 대한 가상 함수도 작성해두자
};
} // namespace queue
} // namespace core