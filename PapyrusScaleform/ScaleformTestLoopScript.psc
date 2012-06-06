Scriptname ScaleformTestLoopScript extends ObjectReference  

Int Counter = 0

Event OnActivate(ObjectReference akActionRef)
	;If (UI.IsMenuOpen("InventoryMenu"))
	;	Debug.Trace("OPEN")
	;	UI.SetBool("InventoryMenu", "_root.Menu_mc.skseWarning._visible", true)
	;	UI.SetString("InventoryMenu", "_root.Menu_mc.skseWarning.message.text", "Counter: " + Counter)
	;EndIf

	Counter = Counter + 1
	Utility.WaitMenuMode(0.01)
	
	RegisterForMenu("InventoryMenu")
	RegisterForKey(37)
	RegisterForModEvent("t1", "OnTestEvent")
	RegisterForModEvent("t2", "OnTestEvent")
	RegisterForModEvent("categoryPressed", "OnUIEvent")
	
	Activate(akActionRef)
EndEvent

Function AppendMenuText(string s)
	string cur = UI.GetString("InventoryMenu", "_root.Menu_mc.skseWarning.message.text")
	UI.SetString("InventoryMenu", "_root.Menu_mc.skseWarning.message.text", cur + "\n" + s)
EndFunction

Event OnMenuOpen(string menuName)
	UI.SetBool("InventoryMenu", "_root.Menu_mc.skseWarning._visible", true)
	UI.SetString("InventoryMenu", "_root.Menu_mc.skseWarning.message.text", "Opened " + menuName)
EndEvent

Event OnMenuClose(string menuName)
	UI.SetBool("InventoryMenu", "_root.Menu_mc.skseWarning._visible", true)
	UI.SetString("InventoryMenu", "_root.Menu_mc.skseWarning.message.text", "Closed " + menuName)
EndEvent

Event OnKeyDown(int keyCode)
	AppendMenuText("Pressed key " + keyCode + ", sending 't1'")
	SendModEvent("t1")
EndEvent

Event OnKeyUp(int keyCode, float holdTime)
	AppendMenuText("Released key " + keyCode + ", held for " + holdTime + ", sending 't2'")
	SendModEvent("t2")
EndEvent

Event OnTestEvent(string eventName, string msg)
	AppendMenuText("Received Papyrus event: " + eventName)
EndEvent

Event OnUIEvent(string eventName, string msg)
	AppendMenuText("Received UI event: " + eventName)
EndEvent
