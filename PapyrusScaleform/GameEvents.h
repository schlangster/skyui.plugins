#pragma once

#include "Utilities.h"
#include "GameTypes.h"
#include "PapyrusVM.h"

template <typename T> class BSTEventSink;

enum EventResult
{
	kEvent_Continue = 0,
	kEvent_Abort
};

// 030
template <typename EventT, typename EventArgT = EventT>
class EventDispatcher
{
	typedef BSTEventSink<EventT> SinkT;

	SimpleLock			lock;				// 000
	tArray<SinkT*>		eventSinks;			// 008
	tArray<SinkT*>		addBuffer;			// 014 - schedule for add
	tArray<SinkT*>		removeBuffer;		// 020 - schedule for remove
	bool				stateFlag;			// 02C - some internal state changed while sending
	char				pad[3];

	MEMBER_FN_PREFIX(EventDispatcher);
	DEFINE_MEMBER_FN(AddEventSink_Internal, void, 0x00456010, SinkT * eventSink);
	DEFINE_MEMBER_FN(RemoveEventSink_Internal, void, 0x008C74B0, SinkT * eventSink);
	DEFINE_MEMBER_FN(SendEvent_Internal, void, 0x0044BE70, EventT * evn);

public:

	EventDispatcher() : stateFlag(false) {}

	void AddEventSink(SinkT * eventSink)		{ CALL_MEMBER_FN(this,AddEventSink_Internal)(eventSink); }
	void RemoveEventSink(SinkT * eventSink)		{ CALL_MEMBER_FN(this,RemoveEventSink_Internal)(eventSink); }
	void SendEvent(EventT * evn)				{ CALL_MEMBER_FN(this,SendEvent_Internal)(evn); }
};
STATIC_ASSERT(sizeof(EventDispatcher<void*>) == 0x30);

// 04
template <typename T>
class BSTEventSink
{
public:
	virtual ~BSTEventSink();
	virtual	EventResult	ReceiveEvent(T * evn, EventDispatcher<T> * dispatcher); // pure
//	void	** _vtbl;	// 00
};

// 30
template <typename T>
class BSTEventSource
{
public:
	virtual ~BSTEventSource();

//	void	** _vtbl;	// 00
	UInt32 unk04[11];	// 04
};

STATIC_ASSERT(sizeof(BSTEventSource<void*>) == 0x30);

// 08
struct TESSleepStartEvent
{
	float startTime;	// 00
	float endTime;		// 04
};

// 08
struct MenuOpenCloseEvent
{
	BSFixedString	menuName;	// 00
	bool			opening;	// 04
	char			pad[3];
};

// Todo
struct MenuModeChangeEvent
{
};

// 10 - See sub_A61970
class InputEvent
{
public:
	enum
{
		kInput_Button = 0,
		kInput_MouseMove,
		kInput_Char,
		kInput_Thumbstick,
		kInput_DeviceConnect,
		kInput_Kinect
	};

	virtual			~InputEvent();
	virtual bool	IsIDEvent();
	virtual UInt32	GetID();

//	void			** _vtbl;	// 00
	UInt32			unk04;		// 04 - flags?
	const UInt32	type;		// 08
	InputEvent *	next;		// 0C
};
STATIC_ASSERT(sizeof(InputEvent) == 0x10);

class IDEvent
{
public:
	UInt32			id;			// 00
};

// 20
class ButtonEvent : public IDEvent, public InputEvent
{
public:
	UInt32			unk14;		// 14
	UInt32			unk18;		// 18
	UInt32			unk1C;		// 1C
};

class MouseMoveEvent : public IDEvent, public InputEvent
{
};

// 14
class CharEvent : public InputEvent
{
public:
	UInt32			keyCode;		// 10
};
STATIC_ASSERT(sizeof(CharEvent) == 0x14);

class ThumbstickEvent : public IDEvent, public InputEvent
{
};

class DeviceConnectEvent : public InputEvent
{
};

class KinectEvent : public IDEvent, public InputEvent
{
};



template <>
class BSTEventSink <TESSleepStartEvent>
{
public:
	virtual ~BSTEventSink() {}; // todo?
	virtual	EventResult ReceiveEvent(TESSleepStartEvent * evn, EventDispatcher<TESSleepStartEvent> * dispatcher) = 0;
};

template <>
class BSTEventSink <MenuOpenCloseEvent>
{
public:
	virtual ~BSTEventSink() {}; // todo?
	virtual	EventResult ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher) = 0;
};

template <>
class BSTEventSink <InputEvent>
{
public:
	virtual ~BSTEventSink() {}; // todo?
	virtual	EventResult ReceiveEvent(InputEvent ** evn, EventDispatcher<InputEvent,InputEvent*> * dispatcher) = 0;
};

// For testing
//extern EventDispatcher<TESSleepStartEvent> * g_sleepStartEventDispatcher;
extern EventDispatcher<InputEvent,InputEvent*> ** g_inputEventDispatcher;