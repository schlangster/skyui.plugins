#include "PapyrusEvents.h"
#include "GameMenus.h"
#include <set>

/*EventResult SKSEEventHandler::ReceiveEvent(TESSleepStartEvent * evn, EventDispatcher<TESSleepStartEvent> * dispatcher)
{
	_MESSAGE("Received TESSleepStartEvent. From: %f, To: %f.", evn->startTime, evn->endTime);
	return kEvent_Continue;
}*/

EventResult SKSEEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
	_MESSAGE("Received MenuOpenCloseEvent. Name: %s, Opening: %d", evn->menuName, evn->opening);

	VMClassRegistry		* registry = (*g_skyrimVM)->GetClassRegistry();

	g_menuOpenCloseRegHolder.Acquire();

	MenuOpenCloseRegTable::iterator	handles = g_menuOpenCloseRegHolder.data.find(evn->menuName.data);
	if (handles != g_menuOpenCloseRegHolder.data.end())
	{
		for(HandleList::iterator iter = handles->second.begin(); iter != handles->second.end(); ++iter)
		{
			OneStringArg	args(evn->menuName.data);

			if (evn->opening)
				registry->QueueEvent(*iter, &BSFixedString("OnMenuOpen"), &args);
			else
				registry->QueueEvent(*iter, &BSFixedString("OnMenuClose"), &args);
		}
	}

	g_menuOpenCloseRegHolder.Release();

	return kEvent_Continue;
}


MenuOpenCloseRegHolder	g_menuOpenCloseRegHolder;
SKSEEventHandler		g_skseEventHandler;
