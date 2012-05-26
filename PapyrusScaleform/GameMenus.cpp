#include "GameMenus.h"

GFxMovieView * MenuManager::GetMovieView(const char * menuName)
{
	// Necessary #1?
	StringCache::Ref menuNameRef;
	CALL_MEMBER_FN(&menuNameRef, ctor)(menuName);

	MenuTableItem * item = menuTable.Find<const char *>(menuNameRef.data);

	CALL_MEMBER_FN(&menuNameRef, Release)();

	if (!item)
		return NULL;

	IMenu * menu = item->menuInstance;
	if (!menu)
		return NULL;

	// Necessary #2?
	menu->AddRef();
	GFxMovieView * view = menu->view;
	menu->Release();

	return view;
}