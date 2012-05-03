#include "PapyrusUI.h"

#include "PapyrusVM.h"
#include "PapyrusNativeFunctions.h"

#include "ScaleformCallbacks.h"
#include "ScaleformMovie.h"

#include "GameMenus.h"

#include "Utilities.h"


// sub_A454D0 - ctor
class MenuManager
{
	struct Result
	{
		IMenu *		menu; // 00
		UInt32		unk004; // 04
		Result() : menu(0), unk004(0) {};
	};

	struct List
	{
		UInt32		unk074;		// 00
		UInt32		unk078;		// 04
		UInt32		unk07C;		// 08
		UInt32		unk080;		// 0C
		char		hash[8];	// 10 end of list? hash?
		UInt32		* data;		// 18 - pointer to list data, passed to GetMenu

		MEMBER_FN_PREFIX(List);
		DEFINE_MEMBER_FN(GetMenu, bool, 0x00A458C0, void * arg1, BSFixedString * arg2, BSFixedString * arg3, Result * arg4);
	};

public:
	char		pad[0x74];
	List		list;			// 074
	UInt32		* unk090;		// 090 - threadId?

	MEMBER_FN_PREFIX(MenuManager);
	DEFINE_MEMBER_FN(IsMenuOpen, bool, 0x00A447B0, BSFixedString * menuName);
	//DEFINE_MEMBER_FN(Register, void, 0x00A44A70, const char * name, void * func);

	static MenuManager *	GetSingleton(void)
	{
		return *((MenuManager **)0x012B8A98);
	}

	GFxMovieView *	GetMovieView(BSFixedString * menuName)
	{
		BSFixedString * magic = menuName;
		Result result;

		typedef void * (__cdecl * _GetHash)(BSFixedString ** arg1, const char * arg2);
		((_GetHash)0x00A32040)(&magic, magic->data);

		CALL_MEMBER_FN(&list, GetMenu)(list.data, magic, menuName, &result);
		IMenu * menu = result.menu;

		if (!menu)
			return NULL;

		GFxMovieView * view = menu->view;

		// If this is not called, menumode goes on forever
		CALL_MEMBER_FN(menu, Unknown1)();

		return view;
	}
};

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
}

bool ExtractTargetData(const char * target, std::string & dest, std::string & name)
{
	// target format: [_global|_root].d.e.s.t.ValueName
	UInt32 lastDelim = 0;
	std::string t(target);

	// Try to forward to last delim (if there even are any)
	for (UInt32 i=0; target[i]; i++)
		if (target[i] == '.')
			lastDelim = i;

	// Need at least 1 delim
	if (lastDelim == 0)
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
	if (CreateObjectRoot(view, dest.c_str()))
		return false;

	// Try again now
	return view->GetVariable(fxDest, dest.c_str());
}

namespace papyrusUI
{
	// Tested
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

	// Tested
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

	// Tested
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
	
	// Tested
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

	// Tested
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

	// Tested
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

	// Tested
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

	// Tested
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

	// Tested
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

	// Tested
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

	// Tested
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
