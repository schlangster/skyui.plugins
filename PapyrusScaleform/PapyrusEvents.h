#pragma once

#include "skse/GameTypes.h"
#include "skse/GameEvents.h"
#include <map>
#include <set>

/*
class MenuRegistrations
{
public:

	BSFixedString	menuName;
	HandleList		handles;

	bool operator==(const MenuRegistrations & rhs) const	{ return menuName == rhs.menuName; }
	bool operator==(const BSFixedString a_menuName) const	{ return menuName == a_menuName; }
	operator BSFixedString() const							{ return menuName; }
};
typedef tHashSet<MenuRegistrations,BSFixedString> MenuOpenCloseRegTable;
*/

template <typename T>
class SafeDataHolder
{
	SimpleLock	lock;
public:
	T			data;

	void	Acquire(void) { lock.Lock(); }
	void	Release(void) { lock.Release(); }
};

typedef std::set<UInt64>					HandleList;
typedef std::map<const char*,HandleList>	NameRegMap;
typedef std::map<UInt32,HandleList>			NumRegMap;

typedef SafeDataHolder<NameRegMap>	MenuOpenCloseRegistrations;
typedef SafeDataHolder<NumRegMap>	InputEventRegistrations;

extern MenuOpenCloseRegistrations	g_menuOpenCloseRegs;
extern InputEventRegistrations		g_inputEventRegs;


class SKSEEventHandler :
//public BSTEventSink <TESSleepStartEvent>,
	public BSTEventSink <MenuOpenCloseEvent>,
	public BSTEventSink <InputEvent>
{

public:
	//virtual EventResult	ReceiveEvent(TESSleepStartEvent * evn, EventDispatcher<TESSleepStartEvent> * dispatcher);
	virtual EventResult		ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
	virtual EventResult		ReceiveEvent(InputEvent ** evns, EventDispatcher<InputEvent,InputEvent*> * dispatcher);
};

extern SKSEEventHandler	g_skseEventHandler;
