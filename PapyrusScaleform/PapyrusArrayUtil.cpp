#include "PapyrusArrayUtil.h"
#include "PapyrusVM.h"
#include "PapyrusNativeFunctions.h"

namespace papyrusArrayUtil
{	
	void RegisterFuncs(VMClassRegistry* registry)
	{
		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, SInt32, VMArray<BSFixedString>, BSFixedString> ("IndexOfString", "ArrayUtil", papyrusArrayUtil::IndexOfT<BSFixedString>, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, SInt32, VMArray<SInt32>, SInt32> ("IndexOfInt", "ArrayUtil", papyrusArrayUtil::IndexOfT<SInt32>, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, SInt32, VMArray<float>, float> ("IndexOfFloat", "ArrayUtil", papyrusArrayUtil::IndexOfT<float>, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, SInt32, VMArray<bool>, bool> ("IndexOfBool", "ArrayUtil", papyrusArrayUtil::IndexOfT<bool>, registry));
	}
}