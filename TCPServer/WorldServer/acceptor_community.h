#pragma once

#include "../Common/acceptor_base.h"

class AcceptorCommunity : public AcceptorBase
{
public:
	AcceptorCommunity();
	virtual ~AcceptorCommunity();

	void Initialize() override;
	void InitSession(std::size_t _max_session) override;

protected:
	SessionBase* AllocSession() override;
	void ReallocSession(SessionBase* _session) override;
};