#pragma once

#include "Utilities.h"
#include "GameTypes.h"

template <typename T>
class EventDispatcher;

// 04
template <typename T>
class BSTEventSink
{
public:
	enum
	{
		kEvent_Continue = 0,
		kEvent_Abort
	};

	virtual ~BSTEventSink() {}; //TODO
	virtual	UInt32	ReceiveEvent(T * evn, EventDispatcher<T> * dispatcher); // pure

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

// 030
template <typename T>
class EventDispatcher
{
	typedef BSTEventSink<T> SinkType;

	SimpleLock				lock;				// 000
	tArray<SinkType*>		eventSinks;			// 008
	tArray<SinkType*>		addBuffer;			// 014 - schedule for add
	tArray<SinkType*>		removeBuffer;		// 020 - schedule for remove
	bool					stateFlag;			// 02C - some internal state changed while sending
	char					pad[3];

	MEMBER_FN_PREFIX(EventDispatcher);
	DEFINE_MEMBER_FN(AddEventSink_Internal, void, 0x00456010, SinkType * eventSink);
	DEFINE_MEMBER_FN(RemoveEventSink_Internal, void, 0x008C74B0, SinkType * eventSink);
	DEFINE_MEMBER_FN(SendEvent_Internal, void, 0x0044BE70, T * evn);

public:
	EventDispatcher() : stateFlag(false) {}

	void AddEventSink(SinkType * eventSink)		{ CALL_MEMBER_FN(this,AddEventSink_Internal)(eventSink); }
	void RemoveEventSink(SinkType * eventSink)	{ CALL_MEMBER_FN(this,RemoveEventSink_Internal)(eventSink); }
	void SendEvent(T * evn)						{ CALL_MEMBER_FN(this,SendEvent_Internal)(evn); }
};
STATIC_ASSERT(sizeof(EventDispatcher<void*>) == 0x30);

// Example
class SleepStartEvent
{
public:
	float startTime;	// 000
	float endTime;		// 004
};

template<>
class BSTEventSink<SleepStartEvent>
{
public:
	virtual ~BSTEventSink() {}; //TODO
	virtual	UInt32	ReceiveEvent(SleepStartEvent * evn, EventDispatcher<SleepStartEvent> * dispatcher) = 0;
};

class CustomSleepEventSink : public BSTEventSink<SleepStartEvent>
{
public:
	UInt32	ReceiveEvent(SleepStartEvent * evn, EventDispatcher<SleepStartEvent> * dispatcher)
	{
		_MESSAGE("Received event. Sleep from %f to %f.", evn->startTime, evn->endTime);
		return 0;
	}
};

extern EventDispatcher<SleepStartEvent> * g_sleepStartEventDispatcher;