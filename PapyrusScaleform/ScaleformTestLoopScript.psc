Scriptname ScaleformTestLoopScript extends ObjectReference  

Int Counter = 0

Event OnActivate(ObjectReference akActionRef)
	If (UI.IsMenuOpen("Inventory Menu"))
		UI.SetBool("Inventory Menu", "_root.Menu_mc.skseWarning._visible", true)
		UI.SetString("Inventory Menu", "_root.Menu_mc.skseWarning.message.text", "Counter: " + Counter)
	EndIf

	Counter = Counter + 1
	Utility.WaitMenuMode(0.01)
	Activate(akActionRef)
EndEvent
