#include <windows.h>

#include "skse/skse_version.h"
#include "skse/PluginAPI.h"
//#include "skse/ScaleformCallbacks.h"
//#include "skse/ScaleformMovie.h"

#include "Translations.h"

IDebugLog	gLog("skyui_extensions.log");

PluginHandle g_pluginHandle = kPluginHandle_Invalid;
//SKSEScaleformInterface* scaleform = NULL;

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
	info->name = "skyui_extensions";
	info->version = 5;

	g_pluginHandle = skse->GetPluginHandle();

	if (skse->isEditor != 0) {
		_ERROR("skyui not necessary in editor!");
		return false;
	}

	if (skse->runtimeVersion != RUNTIME_VERSION_1_4_21_0) {
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
	if (PatchTranslations() == false) {
		_ERROR("Translations patch failure");
		return false;
	}

	// register scaleform callbacks

	// no longer used, but keep commented out for future reference

	//scaleform->Register("gibbed_interface_extensions", RegisterScaleform);

	return true;
}