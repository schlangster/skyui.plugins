#include "PapyrusEvents.h"
#include "GameMenus.h"
#include "GameRTTI.h"
#include <set>
#include <algorithm>


MenuOpenCloseRegistrations	g_menuOpenCloseRegs;
SKSEEventHandler			g_skseEventHandler;
InputEventRegistrations		g_inputEventRegs;

/*EventResult SKSEEventHandler::ReceiveEvent(TESSleepStartEvent * evn, EventDispatcher<TESSleepStartEvent> * dispatcher)
{
	_MESSAGE("Received internal TESSleepStartEvent. From: %f, To: %f.", evn->startTime, evn->endTime);
	return kEvent_Continue;
}*/

class OneStringArgEventQueueFunctor : public IFunctionArguments
{
public:
	OneStringArgEventQueueFunctor(BSFixedString * a_eventName, const char * a_data)
		: eventName(a_eventName), data(a_data) {}

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

class OneIntArgEventQueueFunctor : public IFunctionArguments
{
public:
	OneIntArgEventQueueFunctor(BSFixedString * a_eventName, UInt32 a_data)
		: eventName(a_eventName), data(a_data) {}

	void			operator() (UInt64 handle)
	{
		VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
		registry->QueueEvent(handle, eventName, this);
	}

	virtual bool	Copy(Output * dst)
	{
		dst->Resize(1);
		dst->Get(0)->SetInt(data);

		return true;
	}

private:
	BSFixedString	* eventName;
	UInt32			data;
};


EventResult SKSEEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
	_MESSAGE("Received internal MenuOpenCloseEvent. Name: %s, Opening: %d", evn->menuName, evn->opening);

	g_menuOpenCloseRegs.Acquire();

	NameRegMap::iterator	handles = g_menuOpenCloseRegs.data.find(evn->menuName.data);

	if (handles != g_menuOpenCloseRegs.data.end())
	{
		BSFixedString eventName = evn->opening ? BSFixedString("OnMenuOpen") : BSFixedString("OnMenuClose");
		OneStringArgEventQueueFunctor func(&eventName, evn->menuName.data);
		for_each(handles->second.begin(), handles->second.end(), func);
	}

	g_menuOpenCloseRegs.Release();

	return kEvent_Continue;
}


EventResult	SKSEEventHandler::ReceiveEvent(InputEvent ** evns, EventDispatcher<InputEvent,InputEvent*> * dispatcher)
		{
	if (! *evns)
		return kEvent_Continue;

	// Just copied size from other input code.
	static UInt8	oldState[0x100] = { 0 };

	for (InputEvent * e = *evns; e; e = e->next)
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

					g_menuOpenCloseRegs.Acquire();

					NumRegMap::iterator	handles = g_inputEventRegs.data.find(t->keyCode);

					if (handles != g_inputEventRegs.data.end())
					{
						BSFixedString eventName = BSFixedString("OnKeyDown");
						OneIntArgEventQueueFunctor func(&eventName, t->keyCode);
						for_each(handles->second.begin(), handles->second.end(), func);
					}

					g_menuOpenCloseRegs.Release();
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

	// This is just temp, i know that char != dx scancode
	for (UInt32 i = 0; i< 0x100; i++)
	{
		if (oldState[i] && (--oldState[i] == 0))
		{
			_MESSAGE("KeyUp: %c", t->keyCode);

			g_inputEventRegs.Acquire();

			NumRegMap::iterator	handles = g_inputEventRegs.data.find(i);

			if (handles != g_inputEventRegs.data.end())
			{
				BSFixedString eventName = BSFixedString("OnKeyUp");
				OneIntArgEventQueueFunctor func(&eventName, i);
				for_each(handles->second.begin(), handles->second.end(), func);
			}

			g_inputEventRegs.Release();
		}
	}

	return kEvent_Continue;
}
