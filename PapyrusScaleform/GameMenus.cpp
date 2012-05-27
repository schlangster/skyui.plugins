#include "GameMenus.h"

GFxMovieView * MenuManager::GetMovieView(UInt32 menuID)
{
	// Necessary #1?
	//StringCache::Ref menuNameRef;
	//CALL_MEMBER_FN(&menuNameRef, ctor)(menuName);

	BSFixedString * menuName = LookupMenuName(menuID);
	if (!menuName || !menuName->data)
		return NULL;

	MenuTableItem * item = menuTable.Find<BSFixedString>(*menuName);

	//CALL_MEMBER_FN(&menuNameRef, Release)();

	if (!item)
		return NULL;

	IMenu * menu = item->menuInstance;
	if (!menu)
		return NULL;

	// Necessary #2?
	//menu->AddRef();
	GFxMovieView * view = menu->view;
	//menu->Release();

	return view;
}