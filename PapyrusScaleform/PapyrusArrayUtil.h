#pragma once
#include "GameTypes.h"
#include "PapyrusVM.h"


struct StaticFunctionTag;
class VMClassRegistry;
template <typename T> class VMArray;


namespace papyrusArrayUtil
{
	void RegisterFuncs(VMClassRegistry* registry);

	template <typename T>
	SInt32 IndexOfT(StaticFunctionTag* thisInput, VMArray<T> arr, T val)
	{
		for (SInt32 i=0; i<arr.Length(); i++)
		{
			T t;
			arr.Get(&t,i);
			if (t == val)
				return i;
		}
		return -1;
	}
}
