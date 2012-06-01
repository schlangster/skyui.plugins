Scriptname ScaleformTestScript extends Quest

Weapon Property myWeapon Auto  
ObjectReference Property LoopActivator Auto  

Event OnInit()
  ;UI.SetNumber("Magic Menu", "ValueName", 1)
  ;UI.SetNumber("Magic Menu", "a.ValueName", 1)
  ;UI.SetNumber("Magic Menu", "a.b.c.ValueName", 1)
  
  ;RegisterForUpdate(3)
  Game.GetPlayer().AddItem(myWeapon, 1, true)
  LoopActivator.Activate(Game.GetPlayer())
  
  Debug.Trace("ON INIT CALLED");
  
	;RegisterForKey(107)
	;RegisterForMenu("Inventory Menu");
	
EndEvent

Event OnUpdate()
	;UI.SetNumber("Magic Menu", "a.b.c.ValueName", 1)
EndEvent

Event OnKeyDown(int keyCode)
	Debug.Trace("PRESSED")
EndEvent





String[] Property myMessages  Auto  
