#include "gsc_saveload.hpp"

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
		free(prev);
		prev = tmp;
	}
	current_save[id] = NULL;
}

void gsc_saveload_save(int id) //saveload_save(origin, angles, groundentnum)
{
	jh_save *newsave = (jh_save*)malloc(sizeof(jh_save));
	if(newsave == NULL)
	{
		stackPushInt(-1);
		return;
	}
	newsave->prevsave = current_save[id];
	current_save[id] = newsave;

	if(!stackGetParamVector(0, newsave->origin))
	{
		printf("Illegal argument[0] for saveload_save\n");
		stackPushInt(1);
		return;
	}
	if(!stackGetParamVector(1, newsave->angles))
	{
		printf("Illegal argument[1] for saveload_save\n");
		stackPushInt(2);
		return;
	}
	if(stackGetParamType(2) == STACK_INT)
	{
		int entnum;
		stackGetParamInt(2, &entnum);
		newsave->groundentity = &g_entities[entnum];
	}
	else
		newsave->groundentity = NULL;
	stackPushInt(0);
}

void gsc_saveload_selectsave(int id)
{
	printf("selecting save\n");
	int backwardscount;
	if(!stackGetParamInt(0, &backwardscount))
	{
		printf("Illegal argument[0] for saveload_selectsave\n");
		stackPushInt(1);
		return;
	}
	printf("selecting save backwardscount: %d\n", backwardscount);
	selected_save[id] = current_save[id];
	if(selected_save[id] == NULL)
	{
		printf("does not exist\n");
		stackPushInt(-1);
		return;
	}
	while(backwardscount && selected_save[id] != NULL)
	{
		printf("going back\n");
		backwardscount--;
		selected_save[id] = selected_save[id]->prevsave;
	}
	if(selected_save[id] == NULL)
	{
		printf("does not exist part 2\n");
		stackPushInt(-2);
		return;
	}
	printf("does exist, pushing backwards\n");
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
	printf("origin: %f, %f, %f\n", selected_save[id]->origin[0], selected_save[id]->origin[1], selected_save[id]->origin[2]);
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