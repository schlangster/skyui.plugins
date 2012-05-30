#include "PapyrusEvents.h"
#include "GameMenus.h"
#include "GameRTTI.h"
#include <set>
#include <algorithm>

/*EventResult SKSEEventHandler::ReceiveEvent(TESSleepStartEvent * evn, EventDispatcher<TESSleepStartEvent> * dispatcher)
{
	_MESSAGE("Received TESSleepStartEvent. From: %f, To: %f.", evn->startTime, evn->endTime);
	return kEvent_Continue;
}*/

class OneStringArgEventQueueFunctor : public IFunctionArguments
{
public:
	OneStringArgEventQueueFunctor(BSFixedString * a_eventName, const char * a_data)
		: eventName(a_eventName), data(a_data) { }

	void			operator() (UInt64 handle)
	{
		VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
		registry->QueueEvent(handle, eventName, this);
	}

	virtual bool	Copy(Output * dst)
	{
		dst->Resize(1);
		dst->Get(0)->SetString(data);

		return true;
	}

private:
	BSFixedString	* eventName;
	const char		* data;
};


EventResult SKSEEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
	_MESSAGE("Received MenuOpenCloseEvent. Name: %s, Opening: %d", evn->menuName, evn->opening);

	g_menuOpenCloseRegHolder.Acquire();

	MenuOpenCloseRegTable::iterator	handles = g_menuOpenCloseRegHolder.data.find(evn->menuName.data);

	if (handles != g_menuOpenCloseRegHolder.data.end())
	{
		BSFixedString eventName = evn->opening ? BSFixedString("OnMenuOpen") : BSFixedString("OnMenuClose");
		OneStringArgEventQueueFunctor func(&eventName, evn->menuName.data);
		for_each(handles->second.begin(), handles->second.end(), func);
	}

	g_menuOpenCloseRegHolder.Release();

	return kEvent_Continue;
}

EventResult	SKSEEventHandler::ReceiveEvent(InputEvent ** evn, EventDispatcher<InputEvent,InputEvent*> * dispatcher)
		{
	if (! *evn)
		return kEvent_Continue;

	// Just copied size from other input code.
	static UInt8	oldState[0x100] = { 0 };

	for (InputEvent * e = *evn; e; e = e->next)
	{
		switch(e->type)
		{
			case InputEvent::kInput_Button:
			{
				ButtonEvent * t = DYNAMIC_CAST(e, InputEvent, ButtonEvent);
				//_MESSAGE("kInput_Button");
				//_MESSAGE("\tid: %08X", t->id);
				//_MESSAGE("\t14: %08X", t->unk14);
				//_MESSAGE("\t18: %08X", t->unk18);		00000038 when ALT is pressed, 0000001D when STRG is pressed
				//_MESSAGE("\t1C: %08X", t->unk1C);
				break;
			}
			case InputEvent::kInput_MouseMove:
			{
				MouseMoveEvent * t = DYNAMIC_CAST(e, InputEvent, MouseMoveEvent);
				//_MESSAGE("kInput_MouseMove");
				break;
			}
			case InputEvent::kInput_Char:
			{
				CharEvent * t = DYNAMIC_CAST(e, InputEvent, CharEvent);
				//_MESSAGE("kInput_Char: %c", t->keyCode);

				if (!oldState[t->keyCode])
				{
					_MESSAGE("KeyDown: %c", t->keyCode);
					oldState[t->keyCode] = 2;
				}

				break;
		}
			case InputEvent::kInput_Thumbstick:
			{
				ThumbstickEvent * t = DYNAMIC_CAST(e, InputEvent, ThumbstickEvent);
				//_MESSAGE("kInput_Thumbstick");
				break;
			}
			case InputEvent::kInput_DeviceConnect:
			{
				DeviceConnectEvent * t = DYNAMIC_CAST(e, InputEvent, DeviceConnectEvent);
				//_MESSAGE("kInput_DeviceConnect");
				break;
			}
			case InputEvent::kInput_Kinect:
			{
				KinectEvent * t = DYNAMIC_CAST(e, InputEvent, KinectEvent);
				//_MESSAGE("kInput_Kinect");
				break;
			}
		};
	}

	for (UInt32 i = 0; i< 0x100; i++)
		if (oldState[i])
			if (--oldState[i] == 0)
				_MESSAGE("KeyUp: %c", i);

	return kEvent_Continue;
}

MenuOpenCloseRegHolder	g_menuOpenCloseRegHolder;
SKSEEventHandler		g_skseEventHandler;
