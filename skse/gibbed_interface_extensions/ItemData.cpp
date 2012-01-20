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

#include <windows.h>
#include <math.h>

#include "skse/skse_version.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/SafeWrite.h"
#include "skse/GameForms.h"
#include "skse/GameObjects.h"
#include "skse/GameReferences.h"
#include "skse/GameRTTI.h"
#include "skse/GameAPI.h"

#include "ItemData.h"


// Helper
void RegisterNumber(GFxValue* dst, const char* name, double value)
{
	GFxValue	fxValue;
	fxValue.SetNumber(value);
	CALL_MEMBER_FN(dst, SetMember)(name, &fxValue);
}


// Helper
void RegisterBool(GFxValue* dst, const char* name, bool value)
{
	GFxValue	fxValue;
	fxValue.SetBool(value);
	CALL_MEMBER_FN(dst, SetMember)(name, &fxValue);
}


double round(double r)
{
	return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}


StandardItemData* __stdcall MyGetStandardItemData(StandardItemData* sid, void** callbacks, Item* item, int arg4)
{
	StandardItemData *info = CALL_MEMBER_FN(sid, GetStandardItemData)(callbacks, item, arg4);
	
	TESForm* form = item->form;

	if (*callbacks == NULL || form == NULL)
		return info;

	UInt8 formType = form->formType;

	//_MESSAGE("item %u %f %d", formType, weight, value);

	RegisterBool(&info->fxValue, "extended", true);
	RegisterNumber(&info->fxValue, "formType", (double)formType);

	// Armor
	if (formType == kFormType_TESObjectARMO) {
		TESObjectARMO* armorForm = DYNAMIC_CAST(form, TESForm, TESObjectARMO);

		if (armorForm) {
			ExtPlayerCharacter* pc = static_cast<ExtPlayerCharacter*>(*g_thePlayer);
			double armorValue = CALL_MEMBER_FN(pc, GetItemArmor)(item);
			armorValue = round(armorValue);

			RegisterNumber(&info->fxValue, "armor", armorValue);
		}
	
	// Weapon
	} else if (formType == kFormType_TESObjectWEAP) {
		TESObjectWEAP* weaponForm = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);

		if (weaponForm) {
			UInt8 weaponType = weaponForm->unk0C4.type;
		
			ExtPlayerCharacter* pc = static_cast<ExtPlayerCharacter*>(*g_thePlayer);
			double damage = CALL_MEMBER_FN(pc, GetItemDamage)(item);
			damage = round(damage);

			RegisterNumber(&info->fxValue, "subType", (double)weaponType);
			RegisterNumber(&info->fxValue, "damage", damage);
		}

	// Ammo
	} else if (formType == kFormType_TESAmmo) {
		TESAmmo* ammoForm = DYNAMIC_CAST(form, TESForm, TESAmmo);

		if (ammoForm) {
			ExtPlayerCharacter* pc = static_cast<ExtPlayerCharacter*>(*g_thePlayer);
			double damage = CALL_MEMBER_FN(pc, GetItemDamage)(item);
			damage = round(damage);

			RegisterNumber(&info->fxValue, "damage", damage);
		}
	}

	return info;
}


MagicItemData* __stdcall MyGetMagicItemData(MagicItemData* mid, void** callbacks, TESForm* form, int arg4)
{
	MagicItemData *info = CALL_MEMBER_FN(mid, GetMagicItemData)(callbacks, form, arg4);
	
	if (*callbacks == NULL || form == NULL)
		return info;

	UInt8 formType = form->formType;

	RegisterBool(&info->fxValue, "extended", true);
	RegisterNumber(&info->fxValue, "formType", (double)formType);

	// Spell
	if (formType == kFormType_SpellItem) {
		MagicItem* spellForm = DYNAMIC_CAST(form, TESForm, MagicItem);

		if (spellForm) {
			ExtMagicItem* extSpell = static_cast<ExtMagicItem*>(spellForm);
			ExtMagicItem::EffectItem* effect = CALL_MEMBER_FN(extSpell, GetCostliestEffectItem)(5, false);
			
			if (effect && effect->mgef) {
				int schoolType = effect->mgef->unk38.school;
				int skillLevel = effect->mgef->unk38.unk28;
				RegisterNumber(&info->fxValue, "subType", (double)schoolType);
				RegisterNumber(&info->fxValue, "skillLevel", (double)skillLevel);
			}
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


bool PatchItemData()
{
	UInt32 standardItemCall = 0x0099C761;
	UInt32 magicItemCall = 0x009D4C11;

	// StandardItemData
	unsigned char original1[] = { 0xE8, 0x1A, 0x02, 0x00, 0x00 };
	if (memcmp((void *)standardItemCall, original1, sizeof(original1)) != 0)
		return false;

	// MagicItemData
	unsigned char original2[] = { 0xE8, 0xEA, 0x02, 0x00, 0x00 };
	if (memcmp((void *)magicItemCall, original2, sizeof(original2)) != 0)
		return false;

	WriteRelCall(standardItemCall, (UInt32)&stub_MyGetStandardItemData);
	WriteRelCall(magicItemCall, (UInt32)&stub_MyGetMagicItemData);

	return true;
}