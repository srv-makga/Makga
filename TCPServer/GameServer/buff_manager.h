#include "game_header.h"

class Actor;

// buff
// debuff
// effect = buff + debuff

class BuffManager
{
public:
	BuffManager(Actor* _actor);
	virtual ~BuffManager();

public:
	void Initialize();

	eResult Add(TableIdx_t _idx, Time_t _time);
	eResult Clear(TableIdx_t _idx, Time_t _time);
	eResult ClearAllBuff();
	eResult ClearAllDebuff();

	bool HasEffect(TableIdx_t _idx);

	Count_t BuffCount() const;
	Count_t DebuffCount() const;
};