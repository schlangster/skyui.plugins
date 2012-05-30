#pragma once

#include "skse/GameTypes.h"
#include "skse/GameEvents.h"
#include <set>

typedef std::set<UInt64>					HandleList;
typedef std::map<const char*, HandleList>	MenuOpenCloseRegTable;


class SKSEEventHandler :
//public BSTEventSink <TESSleepStartEvent>,
	public BSTEventSink <MenuOpenCloseEvent>,
	public BSTEventSink <InputEvent>
{

public:
	//virtual EventResult	ReceiveEvent(TESSleepStartEvent * evn, EventDispatcher<TESSleepStartEvent> * dispatcher);
	virtual EventResult		ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
	virtual EventResult		ReceiveEvent(InputEvent ** evn, EventDispatcher<InputEvent,InputEvent*> * dispatcher);
};

template <typename T>
class SafeDataHolder
{
	SimpleLock	lock;
public:
	T			data;

	void	Acquire(void) { lock.Lock(); }
	void	Release(void) { lock.Release(); }
};

typedef SafeDataHolder<MenuOpenCloseRegTable>	MenuOpenCloseRegHolder;

extern MenuOpenCloseRegHolder	g_menuOpenCloseRegHolder;
extern SKSEEventHandler			g_skseEventHandler;
