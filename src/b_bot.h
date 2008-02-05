// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2008 GhostlyDeath
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
// DESCRIPTION:
//      Bot Code
//
//-----------------------------------------------------------------------------

#ifndef __B_BOT_H__
#define __B_BOT_H__

#include "doomdef.h"
#include "doomstat.h"
#include "d_ticcmd.h"
#include "p_mobj.h"
#include "d_player.h"

typedef struct botcontrol_s
{
	ticcmd_t *cmd;
	mobj_t *target;
	player_t *me;
	int node;
} botcontrol_t;

extern int botplayer;
extern botcontrol_t botactions[MAXPLAYERS];

enum
{
	BA_NULL,
	BA_LOOKING,
	BA_ATTACKING,
	BA_GATHERING,
	BA_EXPLORING,
};
	
void B_BuildTicCommand(ticcmd_t* cmd);
void B_Look(botcontrol_t *mind);
void B_Exploring(botcontrol_t *mind);
void B_AttackTarget(botcontrol_t *mind);

#endif /* __B_BOT_H__ */

