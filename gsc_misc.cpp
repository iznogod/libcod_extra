#include "gsc_misc.hpp"

void gsc_getgroundentity(scr_entref_t id)
{
	playerState_t *ps = SV_GameClientNum(id);
	if(ps->groundEntityNum < 1022)
	{
		stackPushEntity(&g_entities[ps->groundEntityNum]);
		return;
	}
	stackPushUndefined();
}