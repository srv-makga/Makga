#include "pch.h"
#include "actor.h"

Result_t Actor::Change(TableIdx_t _index)
{
    if (nullptr == Terrain())
    {
        LOG_ERROR << LOG_ACTOR(this) << " Terrain is nullptr";
        return fb::eResult_TerrainNotIn;
    }

    CREATE_FBB(fbb);
    fbb.Finish(fb::server::CreateRecv_ActorChange(fbb,
        Uid(),
        OffsetActorInfoBase(fbb)
    ));

    Terrain


    return fb::eResult_Success;
}

Result_t Actor::ChangeRollback()
{
    return Result_t();
}
