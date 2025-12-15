#pragma once

#import makga.network.jobhandler;

class JobThread : public makga::network::JobHandler
{
public:
	JobThread();
	virtual ~JobThread();

	bool Initialize() override;
	void Finalize() override;

	void Run();
};