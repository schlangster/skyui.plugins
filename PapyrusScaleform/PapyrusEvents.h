#pragma once

#include "skse/GameTypes.h"
#include "skse/GameEvents.h"
#include <set>

typedef std::set<UInt64>					HandleList;
typedef std::map<const char*, HandleList>	MenuOpenCloseRegTable;

/* Decided to use STL for now
class MenuOpenCloseEventRegs
{
public:
		BSFixedString	menuName;
		HandleList		handles;

		bool operator==(const MenuOpenCloseEventRegs & rhs) const	{ return menuName == rhs.menuName; }
		bool operator==(const BSFixedString a_name) const			{ return menuName == a_name; }
		operator UInt32() const										{ return (UInt32)menuName.data; }
};
typedef tHashSet<UInt32, MenuOpenCloseEventRegs> MenuOpenCloseRegTable;
*/


class SKSEEventHandler :
//public BSTEventSink <TESSleepStartEvent>,
public BSTEventSink <MenuOpenCloseEvent>
{

public:
	//virtual EventResult	ReceiveEvent(TESSleepStartEvent * evn, EventDispatcher<TESSleepStartEvent> * dispatcher);
	virtual EventResult		ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
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
