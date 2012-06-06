scriptname SKI_ItemMenuMainScript extends Quest  

; --- PRIVATE VARIABLES

string		sInventoryMenu =		"InventoryMenu"
string		sMagicMenu =			"MagicMenu"
string		sSetConfigOverride =	"SKI_setConfigOverride"
string		sConfigManager =		"_global.skyui.util.ConfigManager."

int			configOverrideCount
string[]	configOverrideKeys
string[]	configOverrideValues

; ------------------------------------------------------------------------------------------------------

event OnInit()
	configOverrideCount = 0
	configOverrideKeys = new string[128]
	configOverrideValues = new string[128]

	Setup()
endEvent

; Called from OnOnit and OnPlayerLoadGame
function Setup()
	Debug.Trace("SKI_ItemMenuMainScript.Setup()")
	RegisterForMenu(sInventoryMenu)
	RegisterForMenu(sMagicMenu)
	RegisterForModEvent(sSetConfigOverride, "OnSetConfigOveride")
endFunction


; ------------------------------------------------------------------------------------------------------

event OnMenuOpen(string menuName)
	if (menuName == sInventoryMenu)
		GotoState("INVENTORY_OPENED")
		Debug.Trace("SKI_ItemMenuMainScript: Opened Inventory")
		UI.InvokeStringA(sInventoryMenu, sConfigManager + "setExternalOverrideKeys", configOverrideKeys)
		UI.InvokeStringA(sInventoryMenu, sConfigManager + "setExternalOverrideValues", configOverrideValues)
	endif
endEvent

event OnMenuClose(string menuName)
endEvent

event OnSetConfigOveride(string eventName, string overrideKey)
	Debug.Trace("Received override key: " + overrideKey)
	string overrideValue = UI.GetString(sInventoryMenu, sConfigManager + "out_overrides." + overrideKey)
	Debug.Trace("Looked up value: " + overrideValue)
	
	int index = ArrayUtil.IndexOfString(configOverrideKeys, overrideKey)
	if (index != -1)
		Debug.Trace("Existing override")
		configOverrideValues[index] = overrideValue
	else
		Debug.Trace("New override")
		configOverrideKeys[configOverrideCount] = overrideKey
		configOverrideValues[configOverrideCount] = overrideValue
		configOverrideCount = configOverrideCount + 1
	endif
endEvent

; ------------------------------------------------------------------------------------------------------

state INVENTORY_OPENED

	event OnMenuClose(string menuName)
		if (menuName == sInventoryMenu)
			GotoState("")
			Debug.Trace("SKI_ItemMenuMainScript: Closed Inventory")
		endif
	endEvent

endState
