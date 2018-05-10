#include "gsc_saveload.hpp"

#if COMPILE_SAVELOAD == 1

struct jh_save
{
	jh_save *prevsave;
	vec3_t origin;
	vec3_t angles;
	gentity_t *groundentity;
};

jh_save *selected_save[MAX_CLIENTS];
jh_save *current_save[MAX_CLIENTS] = {NULL};

void gsc_saveload_initclient(int id)
{
	selected_save[id] = NULL;
	jh_save *prev = current_save[id];
	while(prev != NULL)
	{
		jh_save *tmp = current_save[id]->prevsave;
		prev free();
		prev = tmp;
	}
	current_save[id] = NULL;
}

void gsc_saveload_save(int id) //saveload_save(origin, angles, groundentnum)
{
	jh_save *newsave = malloc(sizeof(jh_save));
	if(newsave == NULL)
	{
		stackPushInt(1);
		return;
	}
	newsave->prev = current_save[id];
	current_save[id] = newsave;

	vec3_t origin;
	vec3_t angles;
	if(!stackGetParamVector(0, origin))
	{
		printf("Illegal argument[0] for saveload_save\n");
		stackPushInt(1);
		return;
	}
	if(!stackGetParamVector(1, angles))
	{
		printf("Illegal argument[1] for saveload_save\n");
		stackPushInt(2);
		return;
	}
	if(stackGetParamType(2) == STACK_INT)
	{
		int entnum
		stackGetParamInt(2, &entnum);
		newsave->groundentity = &g_entities[entnum];
	}
	else
		newsave->groundentity = NULL;
	stackPushInt(0);
}

void gsc_saveload_selectsave(int id)
{
	int backwardscount;
	if(!stackGetParamInt(0, &backwardscount))
	{
		printf("Illegal argument[0] for saveload_selectsave\n");
		stackPushInt(1);
		return;
	}
	selected_save[id] = current_save[id];
	if(selected_save[id] == NULL)
	{
		stackPushInt(-1);
		return;
	}
	while(backwardscount && selected_save[id] != NULL)
	{
		backwardscount--;
		selected_save[id] = selected_save[id]->prevsave;
	}
	if(selected_save[id] == NULL)
	{
		stackPushInt(-2);
		return;
	}
	stackPushInt(0);
}

void gsc_saveload_getorigin(int id)
{
	if(selected_save[id] == NULL)
	{
		printf("No save selected for getorigin\n");
		stackPushUndefined();
		return;
	}
	stackPushVector(selected_save[id]->origin);
}

void gsc_saveload_getangles(int id)
{
	if(selected_save[id] == NULL)
	{
		printf("No save selected for getangles\n");
		stackPushUndefined();
		return;
	}
	stackPushVector(selected_save[id]->angles);
}

void gsc_saveload_getgroundentity(int id)
{
	if(selected_save[id] == NULL)
	{
		printf("No save selected for getgroundentity\n");
		stackPushUndefined();
		return;
	}
	if(selected_save[id]->groundentity != NULL)
		stackPushEntity(selected_save[id]->groundentity);
	else
		stackPushUndefined();
}

#endif