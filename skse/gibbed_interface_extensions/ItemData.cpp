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

#include <windows.h>
#include <math.h>

#include "skse/skse_version.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"
#include "skse/ScaleformAPI.h"
#include "skse/SafeWrite.h"
#include "skse/GameForms.h"
#include "skse/GameObjects.h"
#include "skse/GameRTTI.h"

#include "ItemData.h"

struct _Addresses
{
	UInt32 PlayerCharacter;
	
	UInt32 GetFormType;
	UInt32 GetFormWeight;

	UInt32 GetItemValue;
	UInt32 GetItemDamage;
	UInt32 GetItemArmor;

	UInt32 UnknEffectFunc1;

	UInt32 GetStandardItemData;
	UInt32 GetMagicItemData;
}
Addresses;
	
struct _Constants
{
	UInt32	FormType_Armor;
	UInt32	FormType_Weapon;
	UInt32	FormType_Ammo;
	UInt32	FormType_AlchemyItem;

	UInt32	FormType_SpellItem;
	UInt32	FormType_TESShout;
	UInt32	FormType_EffectSetting;
}
Constants;

class Item
{
public:
	TESForm			*form;				// 000
};

struct EffectItem
{
	float			magnitude;		// 000
	UInt32			area;			// 004
	UInt32			duration;		// 008
	EffectSetting*	mgef;	// 00C
	//float	cost;  - maybe?
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
};

class MagicItemData
{
public:
	unsigned int	vtable;				// 000	
	unsigned char	unkn_004[0x008];	// 004
	TESForm *		form;				// 00C
	GFxValue		fxValue;			// 010
	// ?
};

// Helper
void RegisterNumber(GFxValue * dst, const char * name, double value)
{
	GFxValue	fxValue;
	fxValue.SetNumber(value);
	CALL_MEMBER_FN(dst, SetMember)(name, &fxValue);
}

// Helper
void RegisterBool(GFxValue * dst, const char * name, bool value)
{
	GFxValue	fxValue;
	fxValue.SetBool(value);
	CALL_MEMBER_FN(dst, SetMember)(name, &fxValue);
}

void SetupAddresses1_3_10(void)
{
	memset(&Addresses, 0xCC, sizeof(Addresses));
	memset(&Constants, 0xCC, sizeof(Constants));

	Addresses.PlayerCharacter = 0x01570334;

	Addresses.GetFormType = 0x00403050;
	Addresses.GetFormWeight = 0x0047FDE0;

	Addresses.GetItemValue = 0x004A91E0;
	Addresses.GetItemDamage = 0x00857CD0;
	Addresses.GetItemArmor = 0x00857990;

	Addresses.UnknEffectFunc1 = 0x0040CBC0;

	Addresses.GetStandardItemData = 0x0099C980;
	Addresses.GetMagicItemData = 0x009D4F00;

	Constants.FormType_Armor = 0x1A;
	Constants.FormType_Weapon = 0x29;
	Constants.FormType_Ammo = 0x2A;
	Constants.FormType_AlchemyItem = 0x2E;

	Constants.FormType_SpellItem = 0x16;
	Constants.FormType_TESShout = 0x77;
	Constants.FormType_EffectSetting = 0x12;
}

double GetItemDamage(Item *item)
{
	UInt32 func = Addresses.GetItemDamage;
	void *pc = *(void **)Addresses.PlayerCharacter;

	__asm
	{
		push item
		mov ecx, pc
		call func
	}
}

double GetItemArmor(Item *item)
{
	UInt32 func = Addresses.GetItemArmor;
	void *pc = *(void **)Addresses.PlayerCharacter;

	__asm
	{
		push item
		mov ecx, pc
		call func
	}
}

EffectSetting * GetCostliestEffect(TESForm *form)
{
	UInt32 func1 = Addresses.UnknEffectFunc1;

	EffectItem * effect = NULL;

	__asm
	{
		push 0
		push 5
		mov ecx, form
		call func1
		mov effect, eax
	}

	if (effect == NULL)
		return NULL;

	return effect->mgef;
}

int GetPotionType(TESForm *form)
{
	EffectSetting* mgef = GetCostliestEffect(form);

	if (mgef == NULL)
		return -1;

	return mgef->unk38.actorValue;
}

int GetMagicSchool(TESForm *form)
{
	__asm
	{
		mov edx, form
		mov eax, [edx]
		mov ecx, form
		mov edx, [eax+0x194]
		call edx
	}
}

int GetMagicSkillLevel(TESForm *form)
{
	EffectSetting* mgef = GetCostliestEffect(form);

	if (mgef == NULL)
		return -1;

	return mgef->unk38.unk28;
}



StandardItemData *GetStandardItemData(StandardItemData *sid, void **callbacks, Item *item, int a4)
{
	UInt32 func = Addresses.GetStandardItemData;
	__asm
	{
		push a4
		push item
		push callbacks
		mov ecx, sid
		call func
	}
}

MagicItemData *GetMagicItemData(MagicItemData *sid, void **callbacks, TESForm *form, int a4)
{
	UInt32 func = Addresses.GetMagicItemData;
	__asm
	{
		push a4
		push form
		push callbacks
		mov ecx, sid
		call func
	}
}

double round(double r)
{
	return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

StandardItemData * __stdcall MyGetStandardItemData(StandardItemData *sid, void **callbacks, Item *item, int a4)
{
	StandardItemData *info = GetStandardItemData(sid, callbacks, item, a4);
	
	TESForm* form = item->form;

	if (*callbacks == NULL || form == NULL)
		return info;

	UInt8 formType = form->formType;

	//_MESSAGE("item %u %f %d", formType, weight, value);

	RegisterBool(&info->fxValue, "extended", true);
	RegisterNumber(&info->fxValue, "formType", (double)formType);

	// Armor
	if (formType == Constants.FormType_Armor) {
		TESObjectARMO* armorForm = DYNAMIC_CAST(form, TESForm, TESObjectARMO);

		if (armorForm) {
			double armorValue = GetItemArmor(item);
			armorValue = round(armorValue);

			RegisterNumber(&info->fxValue, "armor", armorValue);
		}
	
	// Weapon
	} else if (formType == Constants.FormType_Weapon) {
		TESObjectWEAP* weaponForm = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);

		if (weaponForm) {
			UInt8 weaponType = weaponForm->unk0C4.type;
		
			double damage = GetItemDamage(item);
			damage = round(damage);

			RegisterNumber(&info->fxValue, "subType", (double)weaponType);
			RegisterNumber(&info->fxValue, "damage", damage);
		}

	// Ammo
	} else if (formType == Constants.FormType_Ammo) {
		TESAmmo* ammoForm = DYNAMIC_CAST(form, TESForm, TESAmmo);

		if (ammoForm) {
			double damage = GetItemDamage(item);
			damage = round(damage);

			RegisterNumber(&info->fxValue, "damage", damage);
		}
	}

	return info;
}


MagicItemData * __stdcall MyGetMagicItemData(MagicItemData *sid, void **callbacks, TESForm *form, int a4)
{
	MagicItemData *info = GetMagicItemData(sid, callbacks, form, a4);
	
	if (*callbacks == NULL || form == NULL)
		return info;

	UInt8 formType = form->formType;

	RegisterBool(&info->fxValue, "extended", true);
	RegisterNumber(&info->fxValue, "formType", (double)formType);

	// Spell
	if (formType == Constants.FormType_SpellItem) {
		SpellItem* spellForm = DYNAMIC_CAST(form, TESForm, SpellItem);

		if (spellForm) {
			int schoolType = GetMagicSchool(spellForm);
			int skillLevel = GetMagicSkillLevel(spellForm);

			RegisterNumber(&info->fxValue, "subType", (double)schoolType);
			RegisterNumber(&info->fxValue, "skillLevel", (double)skillLevel);
		}
	}

	return info;
}


void __declspec(naked) stub_MyGetStandardItemData(void)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push [ebp+16] ; unknown
		push [ebp+12] ; item
		push [ebp+8] ; localizer?
		push ecx ; this
		call MyGetStandardItemData
		pop ebp
		ret 12
	}
}

void __declspec(naked) stub_MyGetMagicItemData(void)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push [ebp+16] ; unknown
		push [ebp+12] ; item
		push [ebp+8] ; localizer?
		push ecx ; this
		call MyGetMagicItemData
		pop ebp
		ret 12
	}
}

bool patch_ItemData(UInt32 version)
{
	UInt32 standardItemCall = NULL;
	UInt32 magicItemCall = NULL;

	switch (version)
	{
		case RUNTIME_VERSION_1_3_10_0:
		{
			SetupAddresses1_3_10();

			// StandardItemData
			standardItemCall = 0x0099C761;

			unsigned char original1[] = { 0xE8, 0x1A, 0x02, 0x00, 0x00 };
			if (memcmp((void *)standardItemCall, original1, sizeof(original1)) != 0)
			{
				return false;
			}

			// MagicItemData
			magicItemCall = 0x009D4C11;

			unsigned char original2[] = { 0xE8, 0xEA, 0x02, 0x00, 0x00 };
			if (memcmp((void *)magicItemCall, original2, sizeof(original2)) != 0)
			{
				return false;
			}

			break;
		}

		default:
		{
			return false;
		}
	}

	if (standardItemCall)
		WriteRelCall(standardItemCall, (UInt32)&stub_MyGetStandardItemData);

	if (magicItemCall)
		WriteRelCall(magicItemCall, (UInt32)&stub_MyGetMagicItemData);

	return true;
}