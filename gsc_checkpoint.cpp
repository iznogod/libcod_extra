#include "gsc_checkpoint.hpp"

struct JH_checkpoint;
struct JH_checkpointlist;
bool isPassingRadius(int playernum, JH_checkpoint *cp);
bool isPassingArea(int playernum, JH_checkpoint *cp);
bool isPassingSphere(int playernum, JH_checkpoint *cp);

struct JH_checkpoint
{
	int checkpointid;
	vec3_t origin;
	int flags;
	float radiussq;
	JH_checkpointlist *child;
	bool (*isPassing)(int playerid, JH_checkpoint *cp);
	float height;
	vec3_t vec1;
	float vec1length;
	vec3_t vec2;
	float vec2length;
	vec3_t vec3;
	float vec3length;
	JH_checkpoint *bigBrother;
};

struct JH_checkpointlist
{
	JH_checkpoint *cp;
	JH_checkpointlist *next;
};

enum checkpointFlags
{
	CP_RADIUS = 1,
	CP_SPHERE = 2,
	CP_AREA = 4,
	CP_FINISH = 8,
	CP_ONGROUND = 16
};

JH_checkpointlist *startcheckpointlist = NULL;
JH_checkpointlist *checkpoints = NULL;
JH_checkpointlist *playercheckpoints[64];

float normalize(vec3_t vec)
{
	float length = sqrtf(DotProduct(vec, vec));
	vec[0] /= length;
	vec[1] /= length;
	vec[2] /= length;
	return length;
}

void gsc_checkpoint_init()
{
	//loop through all checkpoint list elems in the checkpoints list
	//for each of their corresponding checkpoints, loop through their children array and free the list elements (NOT the checkpoints)
	//Then free the checkpoint itself
	//Then finally free the list elem
	//then init the startcheckpointlist and the checkpoints pointer

	JH_checkpointlist *tmp = checkpoints;
	while(tmp != NULL)
	{
		JH_checkpointlist *tmp2 = tmp->cp->child;
		while(tmp2 != NULL)
		{
			JH_checkpointlist *tmp3 = tmp2->next;
			free(tmp2);
			tmp2 = tmp3;
		}
		tmp2 = tmp->next;
		free(tmp->cp);
		free(tmp);
		tmp = tmp2;
	}
	startcheckpointlist = NULL;
	checkpoints = NULL;
}

void gsc_checkpoint_add()
{
	JH_checkpoint *cp = (JH_checkpoint*)malloc(sizeof(JH_checkpoint));
	JH_checkpointlist *listelem = (JH_checkpointlist*)malloc(sizeof(JH_checkpointlist));

	if(cp == NULL || listelem == NULL)
	{
		//error: cant malloc enough memory to do the thing
		return;
	}

	listelem->cp = cp;
	listelem->next = checkpoints;
	checkpoints = listelem;

	cp->child = NULL;
	cp->isPassing = NULL;
	cp->bigBrother = NULL;
	stackGetParamInt(0, &cp->checkpointid);
	stackGetParamInt(1, &cp->flags);
	if(cp->flags & CP_RADIUS)
	{
		//called as checkpoint_add(cpid, flags, origin, radius, height)
		stackGetParamVector(2, cp->origin);
		stackGetParamFloat(3, &cp->radiussq);
		cp->radiussq = cp->radiussq * cp->radiussq;
		stackGetParamFloat(4, &cp->height);
		cp->isPassing = (bool(*)(int, JH_checkpoint*))isPassingRadius;
	}
	else if(cp->flags & CP_SPHERE)
	{
		//called as checkpoint_add(cpid, flags, origin, radius);
		stackGetParamVector(2, cp->origin);
		stackGetParamFloat(3, &cp->radiussq);
		cp->radiussq = cp->radiussq * cp->radiussq;
		cp->isPassing = (bool(*)(int, JH_checkpoint*))isPassingSphere;
	}
	else if(cp->flags & CP_AREA)
	{
		//called as checkpoint_add(cpid, flags, origin, height, vec1, vec2) in which vec1 and vec2 have a length and are linearly independent
		stackGetParamVector(2, cp->origin);
		stackGetParamFloat(3, &cp->vec3length);
		stackGetParamVector(4, cp->vec1);
		cp->vec1length = normalize(cp->vec1);
		stackGetParamVector(5, cp->vec2);
		cp->vec2length = normalize(cp->vec2);
		CrossProduct(cp->vec1, cp->vec2, cp->vec3);
		cp->isPassing = (bool(*)(int, JH_checkpoint*))isPassingArea;
	}
	else
	{
		//error: unknown cp type
		checkpoints = listelem->next;
		free(listelem);
		free(cp);
	}
}

JH_checkpoint* getcpbyid(int id)
{
	JH_checkpointlist *list = checkpoints;
	while(list != NULL)
	{
		if(list->cp->checkpointid == id)
			return list->cp;
		list = list->next;
	}
	return NULL;
}

void gsc_checkpoint_addchild()
{
	//called as checkpoint_addchild(parentcpid, childcpid)
	int parentcpid;
	int childcpid;
	stackGetParamInt(0, &parentcpid);
	stackGetParamInt(1, &childcpid);
	JH_checkpoint *parent = getcpbyid(parentcpid);
	JH_checkpoint *child = getcpbyid(childcpid);
	if(parent == NULL || child == NULL)
	{
		//error: one of the checkpoints not found;
		return;
	}
	JH_checkpointlist *listelem = (JH_checkpointlist*)malloc(sizeof(JH_checkpointlist));
	if(listelem == NULL)
	{
		//error: not enough memory
		return;
	}
	listelem->next = parent->child;
	parent->child = listelem;
	listelem->cp = child;
}

void gsc_checkpoint_checkpassed(int playernum)
{
	JH_checkpointlist *list = playercheckpoints[playernum];
	JH_checkpoint *passed = NULL;
	while(list != NULL)
	{
		if(list->cp->isPassing(playernum, list->cp))
		{
			passed = list->cp;
			break;
		}
	}
	if(passed != NULL)
	{
		//player passed a checkpoint
		//load next checkpoint
		//hurray
		playercheckpoints[playernum] = list->cp->child;
		//todo: add check for finished and all that niceness, and return a value to cod accordingly.
	}
}

bool isPassingArea(int playernum, JH_checkpoint *cp)
{
	return true;
}

bool isPassingSphere(int playernum, JH_checkpoint *cp)
{
	return true;
}

bool isPassingRadius(int playernum, JH_checkpoint *cp)
{
	return true;
}