Scriptname ScaleformWeaponScript extends ObjectReference  

Event OnEquipped(Actor akActor)
    If (akActor == Game.GetPlayer())
		;If (UI.IsMenuOpen("Inventory Menu"))
		If (UI.GetBool("Inventory Menu", "_root.Menu_mc._visible"))
			;Debug.MessageBox("Height: " + UI.GetNumber("Inventory Menu", "_root.Menu_mc._height"))
			;Debug.MessageBox("Category: " + UI.GetString("Inventory Menu", "_root.Menu_mc.InventoryLists_mc.panelContainer.CategoryLabel.textField.text"))
			;UI.InvokeString("Inventory Menu", "_global.skse.Log", "This is a test")
			UI.SetBool("Inventory Menu", "_root.Menu_mc.skseWarning._visible", true)
			UI.SetString("Inventory Menu", "_root.Menu_mc.skseWarning.message.text", "this is a test")
			UI.SetString("Inventory Menu", "_root.a.b.c.test", "this is a test2")
			string s = UI.GetString("Inventory Menu", "_root.a.b.c.test")
			Debug.MessageBox("Test: " + s)
			
        EndIf
    EndIf
EndEvent
