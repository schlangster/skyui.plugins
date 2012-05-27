#include "PapyrusUI.h"

#include "PapyrusVM.h"
#include "PapyrusNativeFunctions.h"

#include "ScaleformCallbacks.h"
#include "ScaleformMovie.h"

#include "GameMenus.h"

#include "Utilities.h"


namespace papyrusUI
{
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

	void Invoke(StaticFunctionTag* thisInput, UInt32 menuID, BSFixedString targetStr)
	{
		if (!targetStr.data)
			return;

		MenuManager * mm = MenuManager::GetSingleton();
		if (!mm)
			return;

		GFxMovieView * view = mm->GetMovieView(menuID);
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

	bool IsMenuOpen(StaticFunctionTag* thisInput, UInt32 menuID)
	{
		BSFixedString * menuName = MenuManager::LookupMenuName(menuID);
		if (!menuName)
			return 0;

		MenuManager * mm = MenuManager::GetSingleton();
		if (!mm)
			return false;

		return CALL_MEMBER_FN(mm, IsMenuOpen)(menuName);
	}

	void RegisterFuncs(VMClassRegistry* registry)
	{
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, UInt32, BSFixedString, bool> ("SetBool", "UI", papyrusUI::SetT<bool>, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, UInt32, BSFixedString, float> ("SetNumber", "UI", papyrusUI::SetT<float>, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, UInt32, BSFixedString, BSFixedString> ("SetString", "UI", papyrusUI::SetT<BSFixedString>, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, UInt32, BSFixedString> ("GetBool", "UI", papyrusUI::GetT<bool>, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, float, UInt32, BSFixedString> ("GetNumber", "UI", papyrusUI::GetT<float>, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, BSFixedString, UInt32, BSFixedString> ("GetString", "UI", papyrusUI::GetT<BSFixedString>, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, UInt32, BSFixedString> ("Invoke", "UI", papyrusUI::Invoke, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, UInt32, BSFixedString, bool> ("InvokeBool", "UI", papyrusUI::InvokeArgT<bool>, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, UInt32, BSFixedString, float> ("InvokeNumber", "UI", papyrusUI::InvokeArgT<float>, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, UInt32, BSFixedString, BSFixedString> ("InvokeString", "UI", papyrusUI::InvokeArgT<BSFixedString>, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, bool, UInt32> ("IsMenuOpen", "UI", papyrusUI::IsMenuOpen, registry));
	}
}
