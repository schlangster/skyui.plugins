/* Copyright (c) 2011 Rick (rick 'at' gibbed 'dot' us)
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

/* This is a modified version of the original gibbed_interface_extensions.
 * The original source code can be found at
 *	http://svn.gib.me/public/elderscrolls/trunk/mods/skse_plugins/gibbed_interface_extensions/
 */

#include <windows.h>

#include "skse/skse_version.h"
#include "skse/PluginAPI.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"
#include "skse/GameAPI.h"

#include "ItemData.h"
#include "Translations.h"

IDebugLog	gLog("gibbed_interface_extensions.log");

PluginHandle g_pluginHandle = kPluginHandle_Invalid;
SKSEScaleformInterface* scaleform = NULL;

//enum languageIDs {CZECH, ENGLISH, FRENCH, GERMAN, ITALIAN, POLISH, RUSSIAN, SPANISH};

//class SKSEScaleform_GetCurrentLanguageID : public GFxFunctionHandler
//{
//public:
//	virtual void	Invoke(Args * args)
//	{
//		const char* lang = *((const char**)(0x14A2E84 + 4));
//		_MESSAGE("language (%s)", lang);
//
//		if (_stricmp(lang,"czech") == 0) {
//			args->result->SetNumber(CZECH);
//
//		} else if (_stricmp(lang,"english") == 0) {
//			args->result->SetNumber(ENGLISH);
//
//		} else if (_stricmp(lang,"french") == 0) {
//			args->result->SetNumber(FRENCH);
//
//		} else if (_stricmp(lang,"german") == 0) {
//			args->result->SetNumber(GERMAN);
//
//		} else if (_stricmp(lang,"italian") == 0) {
//			args->result->SetNumber(ITALIAN);
//
//		} else if (_stricmp(lang,"polish") == 0) {
//			args->result->SetNumber(POLISH);
//
//		} else if (_stricmp(lang,"russian") == 0) {
//			args->result->SetNumber(RUSSIAN);
//
//		} else if (_stricmp(lang,"spanish") == 0) {
//			args->result->SetNumber(SPANISH);
//		} else {
//			args->result->SetNumber(-1);
//		}
//	}
//};

//bool RegisterScaleform(GFxMovieView* view, GFxValue* root)
//{
//	RegisterFunction <SKSEScaleform_GetCurrentLanguageID>(root, view, "GetCurrentLanguageID");
//
//	return true;
//}


bool plugin_query(const SKSEInterface *skse, PluginInfo *info)
{
	_MESSAGE("query");

	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "gibbed's interface extensions";
	info->version = 4;

	g_pluginHandle = skse->GetPluginHandle();

	if (skse->isEditor != 0) {
		_ERROR("interface extensions not necessary in editor!");
		return false;
	}

	if (skse->runtimeVersion != RUNTIME_VERSION_1_3_10_0) {
		_ERROR("unsupported runtime version");
		return false;
	}

	//scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);
	//if(!scaleform)
	//{
	//	_MESSAGE("couldn't get scaleform interface");
	//	return false;
	//}

	//if(scaleform->interfaceVersion < SKSEScaleformInterface::kInterfaceVersion)
	//{
	//	_MESSAGE("scaleform interface too old (%d expected %d)", scaleform->interfaceVersion, SKSEScaleformInterface::kInterfaceVersion);
	//	return false;
	//}

	_MESSAGE("runtime version = %08X", skse->runtimeVersion);
	return true;
}

bool plugin_load(const SKSEInterface *skse)
{
	if (PatchItemData() == false) {
		_ERROR("ItemData patch failure");
		return false;
	}

	if (PatchTranslations() == false) {
		_ERROR("Translations patch failure");
		return false;
	}

	// register scaleform callbacks

	// no longer used, but keep commented out for future reference

	//scaleform->Register("gibbed_interface_extensions", RegisterScaleform);

	return true;
}