#pragma once

#include "GameForms.h"
#include "GameTypes.h"
#include "GameEvents.h"
#include <map>
#include <set>
#include <algorithm>


template <typename T>
class SafeDataHolder
{
protected:
	SimpleLock	m_lock;
public:
	T			m_data;

	void	Acquire(void) { lock.Lock(); }
	void	Release(void) { lock.Release(); }
};


// crashes otherwise
#pragma pack (push, 4)

template <typename D>
class EventRegistration
{
public:

	UInt64	handle;
	D		params;

	bool operator<(const EventRegistration & rhs) const	{ return handle < rhs.handle; }
};

#pragma pack (pop, 4)

template <typename K, typename D = void*>
class RegistrationMapHolder : public SafeDataHolder<std::map<K,std::set<EventRegistration<D>>>>
{
	typedef std::set<EventRegistration<D>>	RegSet;
	typedef std::map<K,RegSet>				RegMap;

public:

	void Register(K& key, TESForm * form, D * params = NULL)
	{
		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();
		IObjectHandlePolicy	* policy =		registry->GetHandlePolicy();

		EventRegistration<D> reg;
		reg.handle = policy->Create(form->formType, (void *)form);
		if (params)
			reg.params = *params;
		
		m_lock.Lock();

		if (m_data[key].insert(reg).second)
			policy->AddRef(reg.handle);

		m_lock.Release();
	}

	void Unregister(K& key, TESForm * form)
	{
		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();
		IObjectHandlePolicy	* policy =		registry->GetHandlePolicy();

		EventRegistration<D> reg;
		reg.handle = policy->Create(form->formType, (void *)form);

		m_lock.Lock();

		if (m_data[key].erase(reg))
			policy->Release(reg.handle);

		m_lock.Release();
	}

	void UnregisterFromAll(TESForm * form)
	{
		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();
		IObjectHandlePolicy	* policy =		registry->GetHandlePolicy();

		EventRegistration<D> reg;
		reg.handle = policy->Create(form->formType, (void *)form);

		m_lock.Lock();

		for (RegMap::iterator iter = m_data.begin(); iter != m_data.end(); ++iter)
			if (iter->second.erase(reg))
				policy->Release(reg.handle);

		m_lock.Release();
	}

	void UnregisterFromAll(UInt64 handle)
	{
		VMClassRegistry		* registry =	(*g_skyrimVM)->GetClassRegistry();
		IObjectHandlePolicy	* policy =		registry->GetHandlePolicy();

		EventRegistration<D> reg;
		reg.handle = handle;

		m_lock.Lock();

		for (RegMap::iterator iter = m_data.begin(); iter != m_data.end(); ++iter)
			if (iter->second.erase(reg))
				policy->Release(handle);

		m_lock.Release();
	}

	template <typename F>
	void ForEach(K& key, F& functor)
	{
		m_lock.Lock();

		RegMap::iterator handles = m_data.find(key);

		if (handles != m_data.end())
			for_each(handles->second.begin(), handles->second.end(), functor);

		m_lock.Release();
	}

	void Clear(void)
	{
		m_lock.Lock();

		m_data.clear();

		m_lock.Release();
	}
};

struct ModCallbackParameters
{
	BSFixedString callbackName;
};

// Should use BSFixedString for keys instead since const char * might be released otherwise?
// So far I didn't have the problem because menu names are stored permanently
extern RegistrationMapHolder<const char*>						g_menuOpenCloseRegs;
extern RegistrationMapHolder<UInt32>							g_inputEventRegs;
extern RegistrationMapHolder<const char*,ModCallbackParameters>	g_modCallbackRegs;


struct SKSEModCallbackEvent
{
	BSFixedString	eventName;
};

template <>
class BSTEventSink <SKSEModCallbackEvent>
{
public:
	virtual ~BSTEventSink() {}; // todo?
	virtual	EventResult ReceiveEvent(SKSEModCallbackEvent * evn, EventDispatcher<SKSEModCallbackEvent> * dispatcher) = 0;
};

extern EventDispatcher<SKSEModCallbackEvent> g_modCallbackEventDispatcher;


class SKSEEventHandler :
//public BSTEventSink <TESSleepStartEvent>,
	public BSTEventSink <MenuOpenCloseEvent>,
	public BSTEventSink <InputEvent>,
	public BSTEventSink <SKSEModCallbackEvent>
{

public:
	//virtual EventResult	ReceiveEvent(TESSleepStartEvent * evn, EventDispatcher<TESSleepStartEvent> * dispatcher);
	virtual EventResult		ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
	virtual EventResult		ReceiveEvent(InputEvent ** evns, EventDispatcher<InputEvent,InputEvent*> * dispatcher);
	virtual	EventResult		ReceiveEvent(SKSEModCallbackEvent * evn, EventDispatcher<SKSEModCallbackEvent> * dispatcher);
};

extern SKSEEventHandler	g_skseEventHandler;
