#include "PapyrusUI.h"

#include "PapyrusVM.h"
#include "PapyrusNativeFunctions.h"

#include "ScaleformCallbacks.h"
#include "ScaleformMovie.h"

#include "GameMenus.h"
#include "GameTypes.h"

#include "Utilities.h"


// 00C
class MenuTableItem
{
public:

	const char	* name;				// 000
	IMenu		* menuInstance;		// 004	0 if the menu is not currently open
	void		* menuConstructor;	// 008

	static UInt32 CalcHash(const char * a_name)
	{
		UInt32 hash;
		CRC32_4(&hash, (UInt32) a_name);
		return hash;
	}

	bool Matches(const char * a_name) const
	{
		return name == a_name;
	}

	UInt32 GetHash() const
	{
		UInt32 hash;
		CRC32_4(&hash, (UInt32) name);
		return hash;
	}
};

typedef tHashSet<MenuTableItem> MenuTable;


// 008 (=> _Entry 010 (8 item + 4 next + 4 pad)
class HandleTableItem
{
public:

	UInt64		handle;		// 000

	static UInt32 CalcHash(UInt64 * a_handle)
	{
		UInt32 hash;
		CRC32_8(&hash, (UInt64) a_handle);
		return hash;
	}

	UInt32 GetHash() const
	{
		UInt32 hash;
		CRC32_8(&hash, handle);
		return hash;
	}

	bool Equals(HandleTableItem * item) const
	{
		return item->handle == handle;
	}

	bool Matches(UInt64 * a_handle) const
	{
		return handle == *a_handle;
	}
};

typedef tHashSet<HandleTableItem> HandleTable;



// sub_A454D0 - ctor
class MenuManager
{
	// 030
	struct Unknown1
	{
		UInt32		unk_000;	// 000 (= 0)
		UInt32		unk_004;	// 004 (= 0)

		UnkArray	unk_008;	// 008
		UnkArray	unk_014;	// 014
		UnkArray	unk_020;	// 020

		UInt32		unk_02C;	// 02C (= 0)
	};
	STATIC_ASSERT(sizeof(Unknown1) == 0x30);

	// 030
	struct Unknown3
	{
		UInt32		freqLow;	// 000 (= Frequency.LowPart)
		UInt32		freqHigh;	// 004 (= Frequency.HighPart)

		UInt32		unk_008;	// 008 (= 0)
		UInt32		unk_00C;	// 00C (= 0)
		UInt32		unk_010;	// 010 (= 0)
		UInt32		unk_014;	// 014 (= 0)
		UInt32		unk_018;	// 018 (= 0)
		UInt32		unk_01C;	// 018 (= frequency related)
		
		UInt32		unk_020;	// 020
		UInt32		unk_024;	// 024

		UInt32		unk_028;	// 028 (= 0)
		UInt32		unk_02C;	// 02C (= 0)
	};
	STATIC_ASSERT(sizeof(Unknown3) == 0x30);

private:

	UInt32		unk_000;	// 000
	Unknown1	unk_004;	// 004
	Unknown1	unk_034;	// 034
	UnkArray	unk_064;	// 064
	UInt32			unk_070;	// 070
	MenuTable		menuTable;	// 074
	UInt32			unk_090;	// 090 (= 0)	threadId
	UInt32			unk_094;	// 094 (= 0)
	UInt32			unk_098;	// 098 (= 0)
	UInt32			unk_09C;	// 09C (= 0)
	UInt32		unk_0A0;	// 0A0 (= 0)
	UInt32		unk_0A4;	// 0A4 (= 0)
	UInt32		unk_0A8;	// 0A8 (= 0)
	UInt32		unk_0AC;	// 0AC
	Unknown3	unk_0B0;
	bool		unk_0E0;	// 0E0 (= 0)	dbg: 1
	bool		unk_0E1;	// 0E1 (= 0)
	char		pad[2];

public:

	static MenuManager * GetSingleton(void)
	{
		return *((MenuManager **)0x012B8A98);
	}

	GFxMovieView * GetMovieView(BSFixedString * menuName)
	{
		MenuTableItem * item = menuTable.Find<const char *>(menuName->data);
		if (!item)
			return NULL;

		IMenu * menu = item->menuInstance;
		if (!menu)
			return NULL;

		// Necessary?
		menu->AddRef();
		GFxMovieView * view = menu->view;
		menu->Release();

		return view;
	}

	MEMBER_FN_PREFIX(MenuManager);
	DEFINE_MEMBER_FN(IsMenuOpen, bool, 0x00A447B0, BSFixedString * menuName);
	DEFINE_MEMBER_FN(Register, void, 0x00A44A70, const char * name, void * ctorFunc);
};
STATIC_ASSERT(sizeof(MenuManager) == 0xE4);


GFxMovieView * GetMovieView(const char * menuName)
{
	MenuManager * mm = MenuManager::GetSingleton();

	StringCache::Ref menuNameRef;
	CALL_MEMBER_FN(&menuNameRef, ctor)(menuName);

	GFxMovieView * view = mm->GetMovieView(&menuNameRef);

	CALL_MEMBER_FN(&menuNameRef, Release)();
	
	return view;
}

bool CreateObjectRoot(GFxMovieView * view, const char * dest)
{
	std::string s;
	Tokenizer tokens(dest, ".");

	if (tokens.NextToken(s) == -1)
		return false;

	// Invalid root?
	if (s.compare("_global") != 0 && s.compare("_root") != 0)
		return false;

	std::string curDest(s);

	while(tokens.NextToken(s) != -1)
	{
		GFxValue root;
		view->GetVariable(&root, curDest.c_str());
		const char * name = s.c_str();

		if (! root.HasMember(name))
		{
			GFxValue obj;
			view->CreateObject(&obj);
			root.SetMember(name, &obj);
		}

		curDest.append(".");
		curDest.append(s);
	}

	return true;
}

bool ExtractTargetData(const char * target, std::string & dest, std::string & name)
{
	// target format: [_global|_root].d.e.s.t.ValueName
	
	std::string t(target);
	UInt32 lastDelim = t.rfind('.');

	// Need at least 1 delim
	if (lastDelim == std::string::npos)
		return false;

	dest = t.substr(0, lastDelim);
	name = t.substr(lastDelim+1);

	return true;
}

bool PrepareSet(const char * target, GFxMovieView * view, GFxValue * fxDest, std::string & dest, std::string & name)
{
	if (! ExtractTargetData(target, dest, name))
		return false;

	// If dest exists, done
	if (view->GetVariable(fxDest, dest.c_str()))
		return true;

	// Dest has to be created first
	if (!CreateObjectRoot(view, dest.c_str()))
		return false;

	// Try again now
	return view->GetVariable(fxDest, dest.c_str());
}

namespace papyrusUI
{
	void SetBool(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString targetStr, bool value)
	{
		if (!menuName.data || !targetStr.data)
			return;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return;

		GFxValue fxDest;
		std::string valueDest, valueName;
		if (!PrepareSet(targetStr.data, view, &fxDest, valueDest, valueName))
			return;

		GFxValue fxValue;
		fxValue.SetBool(value);
		fxDest.SetMember(valueName.c_str(), &fxValue);
	}

	void SetNumber(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString targetStr, float value)
	{
		if (!menuName.data || !targetStr.data)
			return;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return;

		GFxValue fxDest;
		std::string valueDest, valueName;
		if (!PrepareSet(targetStr.data, view, &fxDest, valueDest, valueName))
			return;

		GFxValue fxValue;
		fxValue.SetNumber(value);
		fxDest.SetMember(valueName.c_str(), &fxValue);
	}

	void SetString(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString targetStr, BSFixedString value)
	{
		if (!menuName.data || !targetStr.data)
			return;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return;

		GFxValue fxDest;
		std::string dest, name;
		if (!PrepareSet(targetStr.data, view, &fxDest, dest, name))
			return;

		GFxValue fxValue;
		view->CreateString(&fxValue, value.data);
		fxDest.SetMember(name.c_str(), &fxValue);
	}
	
	bool GetBool(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString sourceStr)
	{
		if (!menuName.data || !sourceStr.data)
			return false;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return false;

		GFxValue fxResult;
		if (! view->GetVariable(&fxResult, sourceStr.data))
			return false;

		if (fxResult.GetType() != GFxValue::kType_Bool)
			return false;

		return fxResult.GetBool();
	}

	float GetNumber(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString sourceStr)
	{
		if (!menuName.data || !sourceStr.data)
			return 0;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return 0;

		GFxValue fxResult;
		if (! view->GetVariable(&fxResult, sourceStr.data))
			return 0;

		if (fxResult.GetType() != GFxValue::kType_Number)
			return 0;

		return fxResult.GetNumber();
	}

	BSFixedString GetString(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString sourceStr)
	{
		if (!menuName.data || !sourceStr.data)
			return NULL;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return NULL;

		GFxValue fxResult;
		if (! view->GetVariable(&fxResult, sourceStr.data))
			return NULL;

		if (fxResult.GetType() != GFxValue::kType_String)
			return NULL;

		return fxResult.GetString();
	}

	void Invoke(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString targetStr)
	{
		if (!menuName.data || !targetStr.data)
			return;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return;

		std::string dest, name;
		if (! ExtractTargetData(targetStr.data, dest, name))
			return;

		GFxValue fxDest;
		if (! view->GetVariable(&fxDest, dest.c_str()))
			return;

		fxDest.Invoke(name.c_str(), NULL, NULL, 0);
	}

	void InvokeBool(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString targetStr, bool arg)
	{
		if (!menuName.data || !targetStr.data)
			return;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return;

		std::string dest, name;
		if (! ExtractTargetData(targetStr.data, dest, name))
			return;

		GFxValue fxDest;
		if (! view->GetVariable(&fxDest, dest.c_str()))
			return;

		GFxValue args;
		args.SetBool(arg);
		fxDest.Invoke(name.c_str(), NULL, &args, 1);
		
	}

	void InvokeNumber(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString targetStr, float arg)
	{
		if (!menuName.data || !targetStr.data)
			return;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return;

		std::string dest, name;
		if (! ExtractTargetData(targetStr.data, dest, name))
			return;

		GFxValue fxDest;
		if (! view->GetVariable(&fxDest, dest.c_str()))
			return;

		GFxValue args;
		args.SetNumber(arg);
		fxDest.Invoke(name.c_str(), NULL, &args, 1);
	}

	void InvokeString(StaticFunctionTag* thisInput, BSFixedString menuName, BSFixedString targetStr, BSFixedString arg)
	{
		if (!menuName.data || !targetStr.data || !arg.data)
			return;

		GFxMovieView * view = GetMovieView(menuName.data);
		if (!view)
			return;

		std::string dest, name;
		if (! ExtractTargetData(targetStr.data, dest, name))
			return;

		GFxValue fxDest;
		if (! view->GetVariable(&fxDest, dest.c_str()))
			return;

		GFxValue args;
		view->CreateString(&args, arg.data);
		fxDest.Invoke(name.c_str(), NULL, &args, 1);
	}

	bool IsMenuOpen(StaticFunctionTag* thisInput, BSFixedString menuName)
	{
		if (! menuName.data)
			return false;

		MenuManager * mm = MenuManager::GetSingleton();
		StringCache::Ref menuNameRef;

		CALL_MEMBER_FN(&menuNameRef, ctor)(menuName.data);
		bool result = CALL_MEMBER_FN(mm, IsMenuOpen)(&menuNameRef);
		CALL_MEMBER_FN(&menuNameRef, Release)();

		return result;
	}

	void RegisterFuncs(VMClassRegistry* registry)
	{
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, bool> ("SetBool", "UI", papyrusUI::SetBool, registry));
		
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, float> ("SetNumber", "UI", papyrusUI::SetNumber, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, BSFixedString> ("SetString", "UI", papyrusUI::SetString, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString> ("GetBool", "UI", papyrusUI::GetBool, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, float, BSFixedString, BSFixedString> ("GetNumber", "UI", papyrusUI::GetNumber, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString> ("GetString", "UI", papyrusUI::GetString, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, BSFixedString> ("Invoke", "UI", papyrusUI::Invoke, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, bool> ("InvokeBool", "UI", papyrusUI::InvokeBool, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, float> ("InvokeNumber", "UI", papyrusUI::InvokeNumber, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, BSFixedString> ("InvokeString", "UI", papyrusUI::InvokeString, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, bool, BSFixedString> ("IsMenuOpen", "UI", papyrusUI::IsMenuOpen, registry));
	}
}
