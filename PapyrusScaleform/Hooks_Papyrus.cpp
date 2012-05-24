#include "Hooks_Papyrus.h"
#include "SafeWrite.h"
#include "PapyrusNativeFunctions.h"
#include "PapyrusVM.h"
#include "PapyrusArgs.h"
#include "GameAPI.h"
#include "GameObjects.h"
#include "GameReferences.h"

#include "PapyrusActor.h"
#include "PapyrusActorBase.h"
#include "PapyrusArmor.h"
#include "PapyrusBook.h"
#include "PapyrusCell.h"
#include "PapyrusCombatStyle.h"
#include "PapyrusConstructibleObject.h"
#include "PapyrusEnchantment.h"
#include "PapyrusForm.h"
#include "PapyrusGame.h"
#include "PapyrusIngredient.h"
#include "PapyrusInput.h"
#include "PapyrusKeyword.h"
#include "PapyrusMath.h"
#include "PapyrusMisc.h"
#include "PapyrusObjectReference.h"
#include "PapyrusPotion.h"
#include "PapyrusSKSE.h"
#include "PapyrusSpell.h"
#include "PapyrusStringUtil.h"
#include "PapyrusUI.h"
#include "PapyrusWeapon.h"

typedef void (* _RegisterPapyrusFunctions)(VMClassRegistry ** registry);
_RegisterPapyrusFunctions RegisterPapyrusFunctions = (_RegisterPapyrusFunctions)0x008E2DE0;

void RegisterPapyrusFunctions_Hook(VMClassRegistry ** registryPtr)
{
	// call original code
	RegisterPapyrusFunctions(registryPtr);

	VMClassRegistry * registry = *registryPtr;


	// SKSE
	papyrusSKSE::RegisterFuncs(registry);

	// TESForm
	papyrusForm::RegisterFuncs(registry);

	// TESObjectCELL
	papyrusCell::RegisterFuncs(registry);

	// TESObjectARMO
	papyrusArmor::RegisterFuncs(registry);

	// TESSoulGem
	papyrusSoulGem::RegisterFuncs(registry);

	// BGSApparatus
	papyrusApparatus::RegisterFuncs(registry);

	// Math
	papyrusMath::RegisterFuncs(registry);

	// Input
	papyrusInput::RegisterFuncs(registry);

	// ObjectReference
	papyrusObjectReference::RegisterFuncs(registry);

	// Weapon
	papyrusWeapon::RegisterFuncs(registry);

	// CombatStyle
	papyrusCombatStyle::RegisterFuncs(registry);

	// Actor
	papyrusActor::RegisterFuncs(registry);

	// ActorBase (TESNPC)
	papyrusActorBase::RegisterFuncs(registry);

	// Outfit
	papyrusOutfit::RegisterFuncs(registry);

	// Potion
	papyrusPotion::RegisterFuncs(registry);

	// Spell
	papyrusSpell::RegisterFuncs(registry);

	// Spell
	papyrusEnchantment::RegisterFuncs(registry);

	// Ingredient
	papyrusIngredient::RegisterFuncs(registry);

	// StringUtil
	papyrusStringUtil::RegisterFuncs(registry);

	// Keyword
	papyrusKeyword::RegisterFuncs(registry);

	// TESObjectBOOK
	papyrusBook::RegisterFuncs(registry);

	// ConstructibleObject
	papyrusConstructibleObject::RegisterFuncs(registry);

	// Game
	papyrusGame::RegisterFuncs(registry);

	// UI
	papyrusUI::RegisterFuncs(registry);
}

class SaveLoadManager
{
public:
	MEMBER_FN_PREFIX(SaveLoadManager);
	DEFINE_MEMBER_FN(SaveGame_Internal, void, 0x00672460, const char * fileName);
	DEFINE_MEMBER_FN(LoadGame_Internal, void, 0x006748A0, const char * fileName, bool unk1);

	void SaveGame_Hook(const char * fileName)
	{
		_MESSAGE("Executing SaveLoadManager::SaveGame_Hook. Filename: %s", fileName);

		CALL_MEMBER_FN(this, SaveGame_Internal)(fileName);

		_MESSAGE("Executed SaveLoadManager::SaveGame_Hook. Filename: %s", fileName);
	}

	void LoadGame_Hook(const char * fileName, bool unk1)
	{
		_MESSAGE("Executing SaveLoadManager::LoadGame_Hook. Filename: %s", fileName);

		CALL_MEMBER_FN(this, LoadGame_Internal)(fileName, unk1);

		_MESSAGE("Executed SaveLoadManager::LoadGame_Hook. Filename: %s", fileName);
	}
};

void Hooks_Papyrus_Init(void)
{
	//
}

void Hooks_Papyrus_Commit(void)
{
	WriteRelCall(0x008C3A71, (UInt32)RegisterPapyrusFunctions_Hook);

	// Event registration lifecycle
	WriteRelCall(0x008C1CBA, GetFnAddr(&SkyrimVM::OnFormDelete_Hook));
	WriteRelCall(0x008C20E7, GetFnAddr(&SkyrimVM::RevertGlobalData_Hook));
	WriteRelCall(0x008BEE51, GetFnAddr(&SkyrimVM::SaveGlobalData_Hook));
	WriteRelCall(0x008C22B9, GetFnAddr(&SkyrimVM::LoadGlobalData_Hook));
	
	// Load & save
	WriteRelCall(0x0067A5D2, GetFnAddr(&SaveLoadManager::SaveGame_Hook));
	WriteRelCall(0x0067AF15, GetFnAddr(&SaveLoadManager::LoadGame_Hook));
}
