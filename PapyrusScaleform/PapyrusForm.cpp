#include "PapyrusForm.h"

#include "GameForms.h"
#include "GameRTTI.h"
#include "GameRTTI.h"
#include "PapyrusVM.h"
#include "PapyrusEvents.h"

#include <map>
#include <set>

namespace papyrusForm
{
	UInt32 GetType(TESForm* thisForm)
	{
		return (thisForm) ? thisForm->GetFormType() : 0;
	}

	BSFixedString GetName(TESForm* thisForm)
	{
		if (!thisForm)
			return NULL;
		
		TESFullName* pFullName = DYNAMIC_CAST(thisForm, TESForm, TESFullName);
		if (pFullName)
			return pFullName->name.data;
		return NULL;
	}

	void SetName(TESForm* thisForm, BSFixedString nuName)
	{
		TESFullName* pFullName = DYNAMIC_CAST(thisForm, TESForm, TESFullName);
		// is a const string, so have to just reassign it.
		if (pFullName) {
			pFullName->name = nuName;
		}
	}

	float GetWeight(TESForm* thisForm)
	{
		if (!thisForm)
			return 0.0;

		TESWeightForm* pWeight = DYNAMIC_CAST(thisForm, TESForm, TESWeightForm);
		if (pWeight)
			return pWeight->weight;
		return 0.0;
	}

	void SetWeight(TESForm* thisForm, float nuWeight)
	{
		if (!thisForm)
			return;

		TESWeightForm* pWeight = DYNAMIC_CAST(thisForm, TESForm, TESWeightForm);
		if (pWeight)
			pWeight->weight = nuWeight;
	}

	UInt32 GetBaseDamage(TESForm* thisForm)
	{
		if (!thisForm)
			return 0;
		TESAttackDamageForm* pAttack = DYNAMIC_CAST(thisForm, TESForm, TESAttackDamageForm);
		if (pAttack)
			return pAttack->GetAttackDamage();
		return 0;
	}

	UInt32 GetValue(TESForm* thisForm)
	{
		if (!thisForm)
			return 0;
		TESValueForm* pValue = DYNAMIC_CAST(thisForm, TESForm, TESValueForm);
		if (pValue)
			return pValue->value;
		return 0;
	}

	void SetGoldValue(TESForm* thisForm, UInt32 value)
	{
		if (!thisForm)
			return;
		TESValueForm* pValue = DYNAMIC_CAST(thisForm, TESForm, TESValueForm);
		if (pValue)
			pValue->value = value;		
	}

	UInt32 GetNumKeywords(TESForm* thisForm)
	{
		if (!thisForm)
			return 0;
		BGSKeywordForm* pKeywords = DYNAMIC_CAST(thisForm, TESForm, BGSKeywordForm);
		if (pKeywords)
			return pKeywords->numKeywords;
		return 0;
	}

	BGSKeyword* GetNthKeyword(TESForm* thisForm, UInt32 index)
	{
		if (!thisForm)
			return NULL;
		BGSKeywordForm* pKeywords = DYNAMIC_CAST(thisForm, TESForm, BGSKeywordForm);
		if (pKeywords && index < pKeywords->numKeywords) {
			return pKeywords->keywords[index];
		}
		return NULL;
	}

	typedef std::set <UInt64>				HandleList;
	typedef std::map <UInt32, HandleList>	RegisteredKeyMap;

	// ### TODO: clear this on unload
	RegisteredKeyMap	g_regKeyMap;

	void RegisterForKey(TESForm * thisForm, UInt32 key)
	{
		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();
		IObjectHandlePolicy	* policy =		registry->GetHandlePolicy();

		UInt64	handle = policy->Create(thisForm->formType, (void *)thisForm);

		g_regKeyMap[key].insert(handle);
	}

	void UpdateKeys(UInt8 * data)
	{
		static UInt8	oldState[0x100] = { 0 };

		if(!*g_skyrimVM) return;

		// ### this is off because it's a super temp hack

		BSFixedString		eventName("OnKeyDown");
		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();

		for(UInt32 i = 0; i < 0x100; i++)
		{
			if(!oldState[i] && data[i])
			{
				RegisteredKeyMap::iterator	registeredHandles = g_regKeyMap.find(i);
				if(registeredHandles != g_regKeyMap.end())
				{
					for(HandleList::iterator iter = registeredHandles->second.begin(); iter != registeredHandles->second.end(); ++iter)
					{
						OneIntArg	args(i);

						registry->QueueEvent(*iter, &eventName, &args);
					}
				}
			}

			oldState[i] = data[i];
		}
	}

	void RegisterForMenuOpenClose(TESForm * thisForm, UInt32 menuID)
	{
		BSFixedString * menuName = MenuManager::LookupMenuName(menuID);
		if (!menuName)
			return;

		MenuManager * mm = MenuManager::GetSingleton();
		if (!mm)
			return;

		// Will only be added once. TODO move this somewhere else.
		mm->MenuOpenCloseEventDispatcher()->AddEventSink(&g_skseEventHandler);

		if (*g_inputEventDispatcher)
			(*g_inputEventDispatcher)->AddEventSink(&g_skseEventHandler);

		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();
		IObjectHandlePolicy	* policy =		registry->GetHandlePolicy();

		UInt64	handle = policy->Create(thisForm->formType, (void *)thisForm);

		// This probably prevents object from being unloaded once regged for something as described in the wiki.
		// Would have to check if RevertGlobalData cares about this.
		// policy->AddRef(handle);

		g_menuOpenCloseRegHolder.Acquire();
		g_menuOpenCloseRegHolder.data[menuName->data].insert(handle);
		g_menuOpenCloseRegHolder.Release();
	}

	void UnregisterFromMenuOpenClose(TESForm * thisForm, UInt32 menuID)
	{
		BSFixedString * menuName = MenuManager::LookupMenuName(menuID);
		if (!menuName)
			return;

		MenuManager * mm = MenuManager::GetSingleton();
		if (!mm)
			return;

		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();
		IObjectHandlePolicy	* policy =		registry->GetHandlePolicy();

		UInt64	handle = policy->Create(thisForm->formType, (void *)thisForm);

		g_menuOpenCloseRegHolder.Acquire();
		g_menuOpenCloseRegHolder.data[menuName->data].erase(handle);
		g_menuOpenCloseRegHolder.Release();
	}

	void UnregisterFromAllMenuOpenClose(TESForm * thisForm)
	{
		MenuManager * mm = MenuManager::GetSingleton();
		if (!mm)
			return;

		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();
		IObjectHandlePolicy	* policy =		registry->GetHandlePolicy();

		UInt64	handle = policy->Create(thisForm->formType, (void *)thisForm);

		g_menuOpenCloseRegHolder.Acquire();
		for(MenuOpenCloseRegTable::iterator iter = g_menuOpenCloseRegHolder.data.begin(); iter != g_menuOpenCloseRegHolder.data.end(); ++iter)
			iter->second.erase(handle);
		g_menuOpenCloseRegHolder.Release();
	}
}

#include "PapyrusVM.h"
#include "PapyrusNativeFunctions.h"

void papyrusForm::RegisterFuncs(VMClassRegistry* registry)
{
	registry->RegisterFunction(
		new NativeFunction0 <TESForm, UInt32> ("GetType", "Form", papyrusForm::GetType, registry));

	registry->RegisterFunction(
		new NativeFunction0 <TESForm, BSFixedString> ("GetName", "Form", papyrusForm::GetName, registry));

	registry->RegisterFunction(
		new NativeFunction1 <TESForm, void, BSFixedString> ("SetName", "Form", papyrusForm::SetName, registry));

	registry->RegisterFunction(
		new NativeFunction0 <TESForm, float> ("GetWeight", "Form", papyrusForm::GetWeight, registry));

	registry->RegisterFunction(
		new NativeFunction1 <TESForm, void, float> ("SetWeight", "Form", papyrusForm::SetWeight, registry));

	registry->RegisterFunction(
		new NativeFunction1 <TESForm, void, UInt32> ("SetGoldValue", "Form", papyrusForm::SetGoldValue, registry));

	registry->RegisterFunction(
		new NativeFunction0 <TESForm, UInt32> ("GetNumKeywords", "Form", papyrusForm::GetNumKeywords, registry));

	registry->RegisterFunction(
		new NativeFunction1 <TESForm, BGSKeyword *, UInt32> ("GetNthKeyword", "Form", papyrusForm::GetNthKeyword, registry));

	//registry->RegisterFunction(
	//	new NativeFunction1 <TESForm, void, UInt32> ("RegisterForKey", "Form", papyrusForm::RegisterForKey, registry));

	registry->RegisterFunction(
		new NativeFunction1 <TESForm, void, UInt32> ("RegisterForMenuOpenClose", "Form", papyrusForm::RegisterForMenuOpenClose, registry));

	registry->RegisterFunction(
		new NativeFunction1 <TESForm, void, UInt32> ("UnregisterFromMenuOpenClose", "Form", papyrusForm::UnregisterFromMenuOpenClose, registry));

	registry->RegisterFunction(
		new NativeFunction0 <TESForm, void> ("UnregisterFromAllMenuOpenClose", "Form", papyrusForm::UnregisterFromAllMenuOpenClose, registry));
}
