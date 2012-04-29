#include "PapyrusScaleform.h"

#include "PapyrusVM.h"
#include "PapyrusNativeFunctions.h"

#include "ScaleformCallbacks.h"
#include "ScaleformMovie.h"

#include "Utilities.h"


GFxMovieView * GetMovieByName(const char * name)
{
	// TODO
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

bool ExtractTargetData(const char * target, GFxMovieView * view, std::string & dest, std::string & name, bool mergeDestAndName = false)
{
	// target format: MenuName.a.b.c.ValueName
	Console_Print("ExtractTargetData: %s", target);

	UInt32 start = 0;
	UInt32 end = 0;

	while (true)
	{
		// Always need at least menu + name
		if (target[end] == 0)
			return false;

		if (target[end] == '.')
			break;
		end++;
	}

	std::string t(target);
	std::string menuName;

	// (MenuName).a.b.c.ValueName
	menuName = t.substr(0, end);
	Console_Print("Menu: %s", menuName.c_str());
	view = GetMovieByName(menuName.c_str());
	// TODO
	//if (view == NULL)
	//	return false;

	start = ++end;

	// Case 1: Append the rest of target to dest if seperate name is not needed
	if (mergeDestAndName)
	{
		// dest: _global.a.b.c.ValueName
		dest = "_global.";
		dest.append(t.substr(start));
		Console_Print("dest: %s", dest.c_str());
		return true;
	}

	// Try to forward end to last delim if there are any left
	for (UInt32 i=start; target[i]; i++)
		if (target[i] == '.')
			end = i;

	// Case 2: There were no more delims, top-level dest
	if (start == end)
	{
		Console_Print("Attaching to root");
		// dest: _global, name: ValueName
		dest = "_global";
		name = t.substr(start);
		Console_Print("dest: %s", dest.c_str());
		Console_Print("name: %s", name.c_str());
		return true;
	}

	// Case 3: Seperate dest and name for depth>1
	Console_Print("Attaching to sub-object");

	// dest: _global.a.b.c
	dest = "_global.";
	dest.append(t.substr(start, end - start));
	Console_Print("dest: %s", dest.c_str());

	start = ++end;

	// name: ValueName
	name = t.substr(start);
	Console_Print("name: %s", name.c_str());
	
	return true;
}

bool PrepareSet(const char * target, GFxMovieView * view, GFxValue * fxDest, std::string & dest, std::string & name)
{
	if (! ExtractTargetData(target, view, dest, name))
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
	void SetBool(StaticFunctionTag* thisInput, BSFixedString target, bool value)
	{
		if (! target.data)
			return;

		GFxMovieView * view = NULL;
		GFxValue fxDest;
		std::string dest, name;

		if (!PrepareSet(target.data, view, &fxDest, dest, name))
			return;

		GFxValue fxValue;
		fxValue.SetBool(value);
		fxDest.SetMember(name.c_str(), &fxValue);
	}

	void SetNumber(StaticFunctionTag* thisInput, BSFixedString target, float value)
	{
		if (! target.data)
			return;

		GFxMovieView * view = NULL;
		GFxValue fxDest;
		std::string dest, name;

		if (!PrepareSet(target.data, view, &fxDest, dest, name))
			return;

		GFxValue fxValue;
		fxValue.SetNumber(value);
		fxDest.SetMember(name.c_str(), &fxValue);
	}

	void SetString(StaticFunctionTag* thisInput, BSFixedString target, BSFixedString value)
	{
		if (! target.data)
			return;

		GFxMovieView * view = NULL;
		GFxValue fxDest;
		std::string dest, name;

		if (!PrepareSet(target.data, view, &fxDest, dest, name))
			return;

		GFxValue fxValue;
		view->CreateString(&fxValue, value.data);
		fxDest.SetMember(name.c_str(), &fxValue);
	}
	
	bool GetBool(StaticFunctionTag* thisInput, BSFixedString target)
	{
		if (! target.data)
			return false;

		GFxMovieView * view = NULL;
		std::string dest, name;

		if (! ExtractTargetData(target.data, view, dest, name, true))
			return false;

		// TODO
		return false;

		GFxValue fxDest;
		view->GetVariable(&fxDest, dest.c_str());

		if (fxDest.GetType() != GFxValue::kType_Bool)
			return false;

		return fxDest.GetBool();
	}

	float GetNumber(StaticFunctionTag* thisInput, BSFixedString target)
	{
		if (! target.data)
			return 0;

		GFxMovieView * view = NULL;
		std::string dest, name;

		if (! ExtractTargetData(target.data, view, dest, name, true))
			return 0;

		// TODO
		return 0;

		GFxValue fxDest;
		view->GetVariable(&fxDest, dest.c_str());

		if (fxDest.GetType() != GFxValue::kType_Number)
			return 0;

		return fxDest.GetNumber();
	}

	BSFixedString GetString(StaticFunctionTag* thisInput, BSFixedString target)
	{
		if (! target.data)
			return NULL;

		GFxMovieView * view = NULL;
		std::string dest, name;

		if (! ExtractTargetData(target.data, view, dest, name, true))
			return NULL;

		// TODO
		return NULL;

		GFxValue fxDest;
		view->GetVariable(&fxDest, dest.c_str());

		if (fxDest.GetType() != GFxValue::kType_String)
			return NULL;

		return fxDest.GetString();
	}

	void Invoke(StaticFunctionTag* thisInput, BSFixedString target)
	{
		if (! target.data)
			return;

		GFxMovieView * view = NULL;
		std::string dest, name;

		if (! ExtractTargetData(target.data, view, dest, name, true))
			return;

		// TODO
		return;

		GFxValue result, args;
		view->Invoke(name.c_str(), &result, &args, 0);
	}

	void InvokeBool(StaticFunctionTag* thisInput, BSFixedString target, bool arg)
	{
		if (! target.data)
			return;

		GFxMovieView * view = NULL;
		std::string dest, name;

		if (! ExtractTargetData(target.data, view, dest, name, true))
			return;

		// TODO
		return;

		GFxValue result, args;
		args.SetBool(arg);
		view->Invoke(name.c_str(), &result, &args, 1);
	}

	void InvokeNumber(StaticFunctionTag* thisInput, BSFixedString target, float arg)
	{
		if (! target.data)
			return;

		GFxMovieView * view = NULL;
		std::string dest, name;

		if (! ExtractTargetData(target.data, view, dest, name, true))
			return;

		// TODO
		return;

		GFxValue result, args;
		args.SetNumber(arg);
		view->Invoke(name.c_str(), &result, &args, 1);
	}

	void InvokeString(StaticFunctionTag* thisInput, BSFixedString target, BSFixedString arg)
	{
		if (! target.data)
			return;

		GFxMovieView * view = NULL;
		std::string dest, name;

		if (! ExtractTargetData(target.data, view, dest, name, true))
			return;

		// TODO
		return;

		GFxValue result, args;
		view->CreateString(&args, arg.data);
		view->Invoke(name.c_str(), &result, &args, 1);
	}

	void RegisterFuncs(VMClassRegistry* registry)
	{
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, bool> ("SetBool", "Scaleform", papyrusScaleform::SetBool, registry));
		
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, float> ("SetNumber", "Scaleform", papyrusScaleform::SetNumber, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, BSFixedString> ("SetString", "Scaleform", papyrusScaleform::SetString, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, bool, BSFixedString> ("GetBool", "Scaleform", papyrusScaleform::GetBool, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, float, BSFixedString> ("GetNumber", "Scaleform", papyrusScaleform::GetNumber, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, BSFixedString> ("GetString", "Scaleform", papyrusScaleform::GetString, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, void, BSFixedString> ("Invoke", "Scaleform", papyrusScaleform::Invoke, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, bool> ("InvokeBool", "Scaleform", papyrusScaleform::InvokeBool, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, float> ("InvokeNumber", "Scaleform", papyrusScaleform::InvokeNumber, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, BSFixedString> ("InvokeString", "Scaleform", papyrusScaleform::InvokeString, registry));
	}
}
