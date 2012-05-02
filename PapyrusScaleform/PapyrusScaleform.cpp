#include "PapyrusScaleform.h"

#include "PapyrusVM.h"
#include "PapyrusNativeFunctions.h"

#include "ScaleformCallbacks.h"
#include "ScaleformMovie.h"

#include "GameMenus.h"

#include "Utilities.h"


// sub_A454D0 - ctor
class MenuManager
{
public:

	struct Result
	{
		IMenu *		menu; // 00
		UInt32		unk004; // 04

		Result() : menu(0), unk004(0) {};
	};
	
	// 0C
	struct List
	{
		int		unk074;		// 00
		int		unk078;		// 04
		int		unk07C;		// 08
		int		unk080;		// 0C
		char	delim[8];	// 10 end of list? hash?
		void	* data;		// 18 - pointer to list data, passed to GetMenu

		MEMBER_FN_PREFIX(List);

		// sub_A32040 is always called before this. CRC?
		DEFINE_MEMBER_FN(GetMenu, bool, 0x00A458C0, void * data, void * name1, void * name2, Result * result);
	};
	
	char		pad[0x74];
	List		list;	// 074
	int			* unk090;	// 090 - threadId?

	// ...

	MEMBER_FN_PREFIX(MenuManager);
	DEFINE_MEMBER_FN(IsMenuOpen, bool, 0x00A447B0, StringCache::Ref * name);
	//DEFINE_MEMBER_FN(Register, void, 0x00A44A70, const char * name, void * func);

	static MenuManager *	GetSingleton(void)
	{
		return *((MenuManager **)0x012B8A98);
	}
};

GFxMovieView * GetMovieViewByName(const char * name)
{
	// TODO
	Console_Print("Menu: %s", name);
	return NULL;
}

void CreateObjectRoot(GFxMovieView * view, const char * dest)
{
	std::string t;
	Tokenizer tokens(dest, ".");

	GFxValue root;
	view->GetVariable(&root, "_global");

	while(tokens.NextToken(t) != -1)
	{
		const char * name = t.c_str();

		if (! root.HasMember(name))
		{
			GFxValue obj;
			view->CreateObject(&obj);
			root.SetMember(name, &obj);
			root = obj;
			continue;
		}
		
		root.GetMember(name, &root);
	}
}

bool ExtractTargetData(const char * target, std::string & dest, std::string & name, bool mergeDestAndName = false)
{
	// target format: a.b.c.ValueName
	Console_Print("ExtractTargetData: %s", target);

	UInt32 lastDelim = 0;

	std::string t(target);

	// Case 1: dest = _global.target
	if (mergeDestAndName)
	{
		// dest: _global.a.b.c.ValueName
		dest = "_global.";
		dest.append(t);
		Console_Print("dest: %s", dest.c_str());
		return true;
	}

	// Try to forward to last delim (if there even are any)
	for (UInt32 i=0; target[i]; i++)
		if (target[i] == '.')
			lastDelim = i;

	// Case 2: No delims, top-level dest
	if (lastDelim == 0)
	{
		Console_Print("Attaching to root");
		// dest: _global, name: ValueName
		dest = "_global";
		name = t;
		Console_Print("dest: %s", dest.c_str());
		Console_Print("name: %s", name.c_str());
		return true;
	}

	// Case 3: Separate dest and name
	Console_Print("Attaching to sub-object");

	// dest: _global.a.b.c
	dest = "_global.";
	dest.append(t.substr(0, lastDelim));
	Console_Print("dest: %s", dest.c_str());

	// name: ValueName
	name = t.substr(lastDelim+1);
	Console_Print("name: %s", name.c_str());
	
	return true;
}

bool PrepareSet(const char * target, GFxMovieView * view, GFxValue * fxDest, std::string & dest, std::string & name)
{
	if (! ExtractTargetData(target, dest, name))
		return false;

	// TODO
	return false;

	if (! view->GetVariable(fxDest, dest.c_str()))
	{
		CreateObjectRoot(view, dest.c_str());
		if (! view->GetVariable(fxDest, dest.c_str()))
			return false;
	}

	return true;
}

namespace papyrusScaleform
{
	void SetBool(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target, bool value)
	{
		if (!menu.data || !target.data)
			return;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		GFxValue fxDest;
		std::string dest, name;
		if (!PrepareSet(target.data, view, &fxDest, dest, name))
			return;

		GFxValue fxValue;
		fxValue.SetBool(value);
		fxDest.SetMember(name.c_str(), &fxValue);
	}

	void SetNumber(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target, float value)
	{
		if (!menu.data || !target.data)
			return;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		GFxValue fxDest;
		std::string dest, name;
		if (!PrepareSet(target.data, view, &fxDest, dest, name))
			return;

		GFxValue fxValue;
		fxValue.SetNumber(value);
		fxDest.SetMember(name.c_str(), &fxValue);
	}

	void SetString(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target, BSFixedString value)
	{
		if (!menu.data || !target.data)
			return;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		GFxValue fxDest;
		std::string dest, name;
		if (!PrepareSet(target.data, view, &fxDest, dest, name))
			return;

		GFxValue fxValue;
		view->CreateString(&fxValue, value.data);
		fxDest.SetMember(name.c_str(), &fxValue);
	}
	
	bool GetBool(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target)
	{
		if (!menu.data || !target.data)
			return false;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		std::string dest, name;
		if (! ExtractTargetData(target.data, dest, name, true))
			return false;

		// TODO
		return false;

		GFxValue fxDest;
		view->GetVariable(&fxDest, dest.c_str());

		if (fxDest.GetType() != GFxValue::kType_Bool)
			return false;

		return fxDest.GetBool();
	}

	float GetNumber(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target)
	{
		if (!menu.data || !target.data)
			return 0;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		std::string dest, name;
		if (! ExtractTargetData(target.data, dest, name, true))
			return 0;

		// TODO
		return 0;

		GFxValue fxDest;
		view->GetVariable(&fxDest, dest.c_str());

		if (fxDest.GetType() != GFxValue::kType_Number)
			return 0;

		return fxDest.GetNumber();
	}

	BSFixedString GetString(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target)
	{
		if (!menu.data || !target.data)
			return NULL;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		std::string dest, name;
		if (! ExtractTargetData(target.data, dest, name, true))
			return NULL;

		// TODO
		return NULL;

		GFxValue fxDest;
		view->GetVariable(&fxDest, dest.c_str());

		if (fxDest.GetType() != GFxValue::kType_String)
			return NULL;

		return fxDest.GetString();
	}

	void Invoke(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target)
	{
		if (!menu.data || !target.data)
			return;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		std::string dest, name;
		if (! ExtractTargetData(target.data, dest, name, true))
			return;

		// TODO
		return;

		GFxValue result, args;
		view->Invoke(name.c_str(), &result, &args, 0);
	}

	void InvokeBool(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target, bool arg)
	{
		if (!menu.data || !target.data)
			return;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		std::string dest, name;
		if (! ExtractTargetData(target.data, dest, name, true))
			return;

		// TODO
		return;

		GFxValue result, args;
		args.SetBool(arg);
		view->Invoke(name.c_str(), &result, &args, 1);
	}

	void InvokeNumber(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target, float arg)
	{
		if (!menu.data || !target.data)
			return;

		GFxMovieView * view = GetMovieViewByName(menu.data);
		
		std::string dest, name;
		if (! ExtractTargetData(target.data, dest, name, true))
			return;

		// TODO
		return;

		GFxValue result, args;
		args.SetNumber(arg);
		view->Invoke(name.c_str(), &result, &args, 1);
	}

	void InvokeString(StaticFunctionTag* thisInput, BSFixedString menu, BSFixedString target, BSFixedString arg)
	{
		if (!menu.data || !target.data)
			return;

		GFxMovieView * view = GetMovieViewByName(menu.data);

		std::string dest, name;
		if (! ExtractTargetData(target.data, dest, name, true))
			return;

		// TODO
		return;

		GFxValue result, args;
		view->CreateString(&args, arg.data);
		view->Invoke(name.c_str(), &result, &args, 1);
	}

	bool IsMenuOpen(StaticFunctionTag* thisInput, BSFixedString menu)
	{
		if (! menu.data)
			return false;

		// TODO
		return true;
	}

	void RegisterFuncs(VMClassRegistry* registry)
	{
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, bool> ("SetBool", "Scaleform", papyrusScaleform::SetBool, registry));
		
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, float> ("SetNumber", "Scaleform", papyrusScaleform::SetNumber, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, BSFixedString> ("SetString", "Scaleform", papyrusScaleform::SetString, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString> ("GetBool", "Scaleform", papyrusScaleform::GetBool, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, float, BSFixedString, BSFixedString> ("GetNumber", "Scaleform", papyrusScaleform::GetNumber, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString> ("GetString", "Scaleform", papyrusScaleform::GetString, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, BSFixedString> ("Invoke", "Scaleform", papyrusScaleform::Invoke, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, bool> ("InvokeBool", "Scaleform", papyrusScaleform::InvokeBool, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, float> ("InvokeNumber", "Scaleform", papyrusScaleform::InvokeNumber, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, BSFixedString, BSFixedString, BSFixedString> ("InvokeString", "Scaleform", papyrusScaleform::InvokeString, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, bool, BSFixedString> ("IsMenuOpen", "Scaleform", papyrusScaleform::IsMenuOpen, registry));
	}
}
