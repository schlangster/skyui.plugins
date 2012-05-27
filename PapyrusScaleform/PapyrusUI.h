#pragma once

#include "GameTypes.h"
#include "GameMenus.h"
#include "ScaleformCallbacks.h"
#include "ScaleformMovie.h"

struct StaticFunctionTag;
class VMClassRegistry;


template <typename T> void SetGFxValue(GFxValue * val, T arg, GFxMovieView * view)							{ STATIC_ASSERT(false); }
template <> inline void SetGFxValue<bool> (GFxValue * val, bool arg, GFxMovieView * view)					{ val->SetBool(arg); }
template <> inline void SetGFxValue<float> (GFxValue * val, float arg, GFxMovieView * view)					{ val->SetNumber(arg); }
template <> inline void SetGFxValue<BSFixedString> (GFxValue * val, BSFixedString arg, GFxMovieView * view)	{ view->CreateString(val, arg.data); }

template <typename T> T GetGFxValue(GFxValue * val)								{ STATIC_ASSERT(false); }
template <> inline bool GetGFxValue<bool> (GFxValue * val)						{ return (val->GetType() == GFxValue::kType_Bool ? val->GetBool() : false); }
template <> inline float GetGFxValue<float> (GFxValue * val)					{ return (val->GetType() == GFxValue::kType_Number ? val->GetNumber() : 0); }
template <> inline BSFixedString GetGFxValue<BSFixedString> (GFxValue * val)	{ return (val->GetType() == GFxValue::kType_String ? val->GetString() : NULL); }


namespace papyrusUI
{
	void RegisterFuncs(VMClassRegistry* registry);

	bool CreateObjectRoot(GFxMovieView * view, const char * dest);
	bool ExtractTargetData(const char * target, std::string & dest, std::string & name);
	bool PrepareSet(const char * target, GFxMovieView * view, GFxValue * fxDest, std::string & dest, std::string & name);
	
	template <typename T>
	void SetT(StaticFunctionTag* thisInput, UInt32 menuID, BSFixedString targetStr, T value)
	{
		if (!targetStr.data)
			return;

		MenuManager * mm = MenuManager::GetSingleton();
		if (!mm)
			return;

		GFxMovieView * view = mm->GetMovieView(menuID);
		if (!view)
			return;

		GFxValue fxDest;
		std::string valueDest, valueName;
		if (!PrepareSet(targetStr.data, view, &fxDest, valueDest, valueName))
			return;

		GFxValue fxValue;
		SetGFxValue<T>(&fxValue, value, view);
		fxDest.SetMember(valueName.c_str(), &fxValue);
	}

	template <typename T>
	T GetT(StaticFunctionTag* thisInput, UInt32 menuID, BSFixedString sourceStr)
	{
		if (!sourceStr.data)
			return 0;

		MenuManager * mm = MenuManager::GetSingleton();
		if (!mm)
			return 0;

		GFxMovieView * view = mm->GetMovieView(menuID);
		if (!view)
			return 0;

		GFxValue fxResult;
		if (! view->GetVariable(&fxResult, sourceStr.data))
			return 0;

		return GetGFxValue<T>(&fxResult);
	}

	template <typename T>
	void InvokeArgT(StaticFunctionTag* thisInput, UInt32 menuID, BSFixedString targetStr, T arg)
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

		GFxValue args;
		SetGFxValue<T>(&args, arg, view);
		fxDest.Invoke(name.c_str(), NULL, &args, 1);		
	}

	void Invoke(StaticFunctionTag* thisInput, UInt32 menuID, BSFixedString targetStr);
	bool IsMenuOpen(StaticFunctionTag* thisInput, UInt32 menuID);
}