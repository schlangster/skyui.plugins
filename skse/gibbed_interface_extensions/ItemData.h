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

/* This is a modified version of the original gibbed_interface_extensions.
 * The original source code can be found at
 *	http://svn.gib.me/public/elderscrolls/trunk/mods/skse_plugins/gibbed_interface_extensions/
 */

#pragma once


#include "skse/ScaleformCallbacks.h"
#include "skse/GameForms.h"
#include "skse/GameObjects.h"
#include "skse/GameReferences.h"


enum FormType {
	kFormType_NONE = 0,
	kFormType_TES4,
	kFormType_GRUP,
	kFormType_GMST,
	kFormType_KYWD,
	kFormType_LCRT,
	kFormType_AACT,
	kFormType_TXST,
	kFormType_MICN,
	kFormType_GLOB,
	kFormType_CLAS,
	kFormType_FACT,
	kFormType_HDPT,
	kFormType_EYES,
	kFormType_RACE,
	kFormType_SOUN,
	kFormType_ASPC,
	kFormType_SKIL,
	kFormType_EffectSetting, // 12
	kFormType_SCPT,
	kFormType_LTEX,
	kFormType_ENCH,
	kFormType_SpellItem, // 16
	kFormType_SCRL,	
	kFormType_ACTI,
	kFormType_TACT,
	kFormType_TESObjectARMO, // 1A
	kFormType_BOOK,
	kFormType_CONT,
	kFormType_DOOR,
	kFormType_INGR,
	kFormType_LIGH,
	kFormType_MISC,
	kFormType_APPA,
	kFormType_STAT,
	kFormType_SCOL,
	kFormType_MSTT,
	kFormType_GRAS,
	kFormType_TREE,
	kFormType_FLOR,
	kFormType_FURN,
	kFormType_TESObjectWEAP, // 29
	kFormType_TESAmmo, // 2A
	kFormType_NPC_,
	kFormType_LVLN,
	kFormType_KEYM,
	kFormType_AlchemyItem, // 2E
	kFormType_IDLM,
	kFormType_NOTE,
	kFormType_COBJ,
	kFormType_PROJ,
	kFormType_HAZD,
	kFormType_SLGM,
	kFormType_LVLI,
	kFormType_WTHR,
	kFormType_CLMT,
	kFormType_SPGD,
	kFormType_RFCT,
	kFormType_REGN,
	kFormType_NAVI,
	kFormType_CELL,
	kFormType_REFR,
	kFormType_ACHR,
	kFormType_PMIS,
	kFormType_PARW,
	kFormType_PGRE,
	kFormType_PBEA,
	kFormType_PFLA,
	kFormType_PCON,
	kFormType_PBAR,
	kFormType_PHZD,
	kFormType_WRLD,
	kFormType_LAND,
	kFormType_NAVM,
	kFormType_TLOD,
	kFormType_DIAL,
	kFormType_INFO,
	kFormType_QUST,
	kFormType_IDLE,
	kFormType_PACK,
	kFormType_CSTY,
	kFormType_LSCR,
	kFormType_LVSP,
	kFormType_ANIO,
	kFormType_WATR,
	kFormType_EFSH,
	kFormType_TOFT,
	kFormType_EXPL,
	kFormType_DEBR,
	kFormType_IMGS,
	kFormType_IMAD,
	kFormType_FLST,
	kFormType_PERK,
	kFormType_BPTD,
	kFormType_ADDN,
	kFormType_AVIF,
	kFormType_CAMS,
	kFormType_CPTH,
	kFormType_VTYP,
	kFormType_MATT,
	kFormType_IPCT,
	kFormType_IPDS,
	kFormType_ARMA,
	kFormType_ECZN,
	kFormType_LCTN,
	kFormType_MESH,
	kFormType_RGDL,
	kFormType_DOBJ,
	kFormType_LGTM,
	kFormType_MUSC,
	kFormType_FSTP,
	kFormType_FSTS,
	kFormType_SMBN,
	kFormType_SMQN,
	kFormType_SMEN,
	kFormType_DLBR,
	kFormType_MUST,
	kFormType_DLVW,
	kFormType_WOOP,
	kFormType_TESShout, //77
	kFormType_EQUP,
	kFormType_RELA,
	kFormType_SCEN,
	kFormType_ASTP,
	kFormType_OTFT,
	kFormType_ARTO,
	kFormType_MATO,
	kFormType_MOVT,
	kFormType_SNDR,
	kFormType_DUAL,
	kFormType_SNCT,
	kFormType_SOPM,
	kFormType_COLL,
	kFormType_CLFM,
	kFormType_REVB
};


class Item
{
public:
	TESForm			*form;				// 000
};


class ExtPlayerCharacter : public PlayerCharacter
{
public:
	MEMBER_FN_PREFIX(ExtPlayerCharacter);
	DEFINE_MEMBER_FN(GetItemDamage, double, 0x00857CD0, Item *item);
	DEFINE_MEMBER_FN(GetItemArmor, double, 0x00857990, Item *item);
};


class ExtMagicItem : public MagicItem
{
public:
	MEMBER_FN_PREFIX(ExtMagicItem);
	DEFINE_MEMBER_FN(GetCostliestEffectItem, EffectItem*, 0x0040CBC0, int arg1, bool arg2);
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
	DEFINE_MEMBER_FN(GetStandardItemData, StandardItemData*, 0x0099C980,  void** callbacks, Item* item, int arg4);
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
	DEFINE_MEMBER_FN(GetMagicItemData, MagicItemData*, 0x009D4F00, void** callbacks, TESForm* form, int arg4);
};


bool patch_ItemData(UInt32 version);
