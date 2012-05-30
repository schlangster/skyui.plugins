#include "GameMenus.h"

GFxMovieView * MenuManager::GetMovieView(UInt32 menuID)
{
	BSFixedString * menuName = LookupMenuName(menuID);
	if (!menuName || !menuName->data)
		return NULL;

	MenuTableItem * item = menuTable.Find(menuName);

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