#pragma once

class Stat
{
public:
	Stat();
	virtual ~Stat();

	void CopyTo(std::shared_ptr<Stat> other) const;
};