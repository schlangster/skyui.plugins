#pragma once

#include "GameTypes.h"

struct StaticFunctionTag;
class VMClassRegistry;

namespace papyrusScaleform
{
	void RegisterFuncs(VMClassRegistry* registry);

	void SetBool(StaticFunctionTag* thisInput, BSFixedString target, bool value);
	void SetNumber(StaticFunctionTag* thisInput, BSFixedString target, float value);
	void SetString(StaticFunctionTag* thisInput, BSFixedString target, BSFixedString value);
	
	bool GetBool(StaticFunctionTag* thisInput, BSFixedString target);
	float GetNumber(StaticFunctionTag* thisInput, BSFixedString target);
	BSFixedString GetString(StaticFunctionTag* thisInput, BSFixedString target);

	void Invoke(StaticFunctionTag* thisInput, BSFixedString target);
	void InvokeBool(StaticFunctionTag* thisInput, BSFixedString target, bool arg);
	void InvokeNumber(StaticFunctionTag* thisInput, BSFixedString target, float arg);
	void InvokeString(StaticFunctionTag* thisInput, BSFixedString target, BSFixedString arg);
}