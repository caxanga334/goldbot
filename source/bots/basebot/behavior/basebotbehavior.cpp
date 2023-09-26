#include <extdll.h>
#include <meta_api.h>
#include "interfaces/vision.h"
#include "bots/basebot.h"
#include "basebotbehavior.h"

ActionResult<CBaseBot> CBaseBotMainAction::OnStart(CBaseBot* me, Action<CBaseBot>* priorAction)
{
    return Continue();
}

ActionResult<CBaseBot> CBaseBotMainAction::Update(CBaseBot* me, float interval)
{
    return Continue();
}

EventDesiredResult<CBaseBot> CBaseBotMainAction::OnSight(CBaseBot* me, CMemoryEntity* them)
{
    LOG_CONSOLE(PLID, "%3.2f: %s received OnSight Event!", gpGlobals->time, GetName()); 
    return TryContinue();
}
