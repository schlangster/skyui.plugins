/* Copyright (c) 2011 Rick (rick 'at' gibbed 'dot' us)
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#pragma once

#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"
#include "skse/ScaleformAPI.h"
#include "skse/GameForms.h"
#include "skse/GameObjects.h"
#include "skse/GameRTTI.h"
#include "skse/GameAPI.h"
#include "skse/GameReferences.h"


class Item
{
public:
	TESForm			*form;				// 000
};

class StandardItemData
{
public:
	unsigned int	vtable;		// 000
	Item *			item;		// 004
	void *			unknown08;	// 008
	unsigned int	unknown0C;	// 00C
	GFxValue		fxValue;	// 010
	// ?

	MEMBER_FN_PREFIX(StandardItemData);
	DEFINE_MEMBER_FN(GetStandardItemData, StandardItemData*, 0x0099C980,  void **callbacks, Item *item, int a4);
};

class MagicItemData
{
public:
	unsigned int	vtable;				// 000	
	unsigned char	unkn_004[0x008];	// 004
	TESForm *		form;				// 00C
	GFxValue		fxValue;			// 010
	// ?

	MEMBER_FN_PREFIX(MagicItemData);
	DEFINE_MEMBER_FN(GetMagicItemData, MagicItemData*, 0x009D4F00, void **callbacks, TESForm *form, int a4);
};

bool patch_ItemData(UInt32 version);

