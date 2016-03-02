/*
#include "actor.h"
#include "m_random.h"
#include "a_action.h"
#include "p_local.h"
#include "p_enemy.h"
#include "s_sound.h"
#include "thingdef/thingdef.h"
*/

static FRandom pr_inq ("Inquisitor");

DEFINE_ACTION_FUNCTION(AActor, A_InquisitorWalk)
{
	PARAM_ACTION_PROLOGUE;

	S_Sound (self, CHAN_BODY, "inquisitor/walk", 1, ATTN_NORM);
	A_Chase (stack, self);
	return 0;
}

bool InquisitorCheckDistance (AActor *self)
{
	if (self->reactiontime == 0 && P_CheckSight (self, self->target))
	{
		return self->AproxDistance (self->target) < 264*FRACUNIT;
	}
	return false;
}

DEFINE_ACTION_FUNCTION(AActor, A_InquisitorDecide)
{
	PARAM_ACTION_PROLOGUE;

	if (self->target == NULL)
		return 0;

	A_FaceTarget (self);
	if (!InquisitorCheckDistance (self))
	{
		self->SetState (self->FindState("Grenade"));
	}
	if (self->target->Z() != self->Z())
	{
		if (self->Top() + 54*FRACUNIT < self->ceilingz)
		{
			self->SetState (self->FindState("Jump"));
		}
	}
	return 0;
}

DEFINE_ACTION_FUNCTION(AActor, A_InquisitorAttack)
{
	PARAM_ACTION_PROLOGUE;

	AActor *proj;

	if (self->target == NULL)
		return 0;

	A_FaceTarget (self);

	self->AddZ(32*FRACUNIT);
	self->angle -= ANGLE_45/32;
	proj = P_SpawnMissileZAimed (self, self->Z(), self->target, PClass::FindActor("InquisitorShot"));
	if (proj != NULL)
	{
		proj->velz += 9*FRACUNIT;
	}
	self->angle += ANGLE_45/16;
	proj = P_SpawnMissileZAimed (self, self->Z(), self->target, PClass::FindActor("InquisitorShot"));
	if (proj != NULL)
	{
		proj->velz += 16*FRACUNIT;
	}
	self->AddZ(-32*FRACUNIT);
	return 0;
}

DEFINE_ACTION_FUNCTION(AActor, A_InquisitorJump)
{
	PARAM_ACTION_PROLOGUE;

	fixed_t dist;
	fixed_t speed;
	angle_t an;

	if (self->target == NULL)
		return 0;

	S_Sound (self, CHAN_ITEM|CHAN_LOOP, "inquisitor/jump", 1, ATTN_NORM);
	self->AddZ(64*FRACUNIT);
	A_FaceTarget (self);
	an = self->angle >> ANGLETOFINESHIFT;
	speed = self->Speed * 2/3;
	self->velx += FixedMul (speed, finecosine[an]);
	self->vely += FixedMul (speed, finesine[an]);
	dist = self->AproxDistance (self->target);
	dist /= speed;
	if (dist < 1)
	{
		dist = 1;
	}
	self->velz = (self->target->Z() - self->Z()) / dist;
	self->reactiontime = 60;
	self->flags |= MF_NOGRAVITY;
	return 0;
}

DEFINE_ACTION_FUNCTION(AActor, A_InquisitorCheckLand)
{
	PARAM_ACTION_PROLOGUE;

	self->reactiontime--;
	if (self->reactiontime < 0 ||
		self->velx == 0 ||
		self->vely == 0 ||
		self->Z() <= self->floorz)
	{
		self->SetState (self->SeeState);
		self->reactiontime = 0;
		self->flags &= ~MF_NOGRAVITY;
		S_StopSound (self, CHAN_ITEM);
		return 0;
	}
	if (!S_IsActorPlayingSomething (self, CHAN_ITEM, -1))
	{
		S_Sound (self, CHAN_ITEM|CHAN_LOOP, "inquisitor/jump", 1, ATTN_NORM);
	}
	return 0;
}

DEFINE_ACTION_FUNCTION(AActor, A_TossArm)
{
	PARAM_ACTION_PROLOGUE;

	AActor *foo = Spawn("InquisitorArm", self->PosPlusZ(24*FRACUNIT), ALLOW_REPLACE);
	foo->angle = self->angle - ANGLE_90 + (pr_inq.Random2() << 22);
	foo->velx = FixedMul (foo->Speed, finecosine[foo->angle >> ANGLETOFINESHIFT]) >> 3;
	foo->vely = FixedMul (foo->Speed, finesine[foo->angle >> ANGLETOFINESHIFT]) >> 3;
	foo->velz = pr_inq() << 10;
	return 0;
}

