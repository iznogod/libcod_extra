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

void gsc_jump_clearstate_extended(scr_entref_t id)
{
	playerState_t *ps = SV_GameClientNum(id);
	ps->pm_flags &= 0xFFF7FFFF;
	ps->pm_time = 0;
	ps->jumpTime = 0; //to reset wallspeed effects
}

void gsc_renice()
{
	int renicevalue;
	stackGetParamInt(0, &renicevalue);
	char renice[512];
	snprintf(renice, sizeof(renice), "sudo renice %d -p %d", renicevalue, getpid());
	setenv("LD_PRELOAD", "", 1);
	system(renice);
}