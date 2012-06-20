scriptname SKI_WidgetManager extends SKI_QuestBase  

; PRIVATE VARIABLES ---------------------

string	_hudMenu = "HUD Menu"
SKI_WidgetClient[]	_widgetClients
string[]			_widgetTypes
int					_curWidgetID
int					_widgetCount

; PROPERTIES ----------------------------

; FUNCTIONS -------------------------

function CleanUp()
	Debug.Trace("SKI_WidgetManager: CleanUp()")
	string[] args = new string[2]
	_widgetCount = 0
	int i = 0
	while (i < _widgetClients.length)
		if (_widgetClients[i] == none)
			_widgetTypes[i] = none
		else
			_widgetCount += 1
		endIf
		
		i += 1
	endWhile
endFunction

int function RequestWidgetID(SKI_WidgetClient a_client)
	Debug.Trace("SKI_WidgetManager: RequestWidgetID(a_client = " + a_client + ")")
	if (_widgetCount >= 128)
		return -1 ;TODO 20120919 need to check for these
	endIf

	int widgetID = NextWidgetID()
	_widgetClients[widgetID] = a_client
	_widgetCount += 1
	
	Debug.Trace("SKI_WidgetManager: RequestWidgetID() returning " + widgetID as string)
	return widgetID
endFunction

int function NextWidgetID()
	Debug.Trace("SKI_WidgetManager: NextWidgetID()")
	int startIdx = _curWidgetID
	
	while (_widgetClients[_curWidgetID] != none)
		_curWidgetID += 1
		if (_curWidgetID >= 128)
			_curWidgetID = 0
		endIf
		if (_curWidgetID == startIdx)
			return -1 ; Should never happen because we have widgetCount. Just to be sure. 
		endIf
	endWhile
	
	Debug.Trace("SKI_WidgetManager: NextWidgetID returned " + _curWidgetID)
	return _curWidgetID
endFunction

function CreateWidget(int a_widgetID, string a_widgetType)
	Debug.Trace("SKI_WidgetManager: CreateWidget(a_widgetID = " + a_widgetID + ", a_widgetType = " + a_widgetType + ")")
	_widgetTypes[a_widgetID] = a_widgetType
	string[] args = new string[2]
	args[0] = a_widgetID as string
	args[1] = a_widgetType
	UI.InvokeStringA(_hudMenu, "_global.WidgetLoader.loadWidget", args);
endFunction

; EVENTS ----------------------------

event OnInit()
	Debug.Trace("SKI_WidgetManager: OnInit")
	_widgetClients	= new SKI_WidgetClient[128]
	_widgetTypes	= new string[128]
	_curWidgetID	= 0
	_widgetCount	= 0
	
	OnGameReload()
endEvent

event OnGameReload()
	Debug.Trace("SKI_WidgetManager: OnGameReload()")
	RegisterForModEvent("widgetLoaded", "OnWidgetLoaded")
	
	CleanUp()
	
	; Load already registered widgets
	UI.InvokeStringA(_hudMenu, "_global.WidgetLoader.loadWidgets", _widgetTypes);
endEvent

event OnWidgetLoaded(string a_eventName, String a_msg)
	Debug.Trace("SKI_WidgetManager: event OnWidgetLoaded(a_eventName = " + a_eventName + ", a_msg = " + a_msg + ")")
	int widgetID = a_msg as int
	SKI_WidgetClient client = _widgetClients[widgetID]
	
	if (client != none)
		SetWidgetClientInfo(widgetID, client as string)
		SetWidgetPositionX(widgetID, client.X)
		SetWidgetPositionY(widgetID, client.Y)
		SetWidgetAlpha(widgetID, client.Alpha)
		
		client.OnWidgetReset() ; do any custom reset actions here
		
		SetWidgetModes(widgetID, client.Modes) ;Resets the current mode, but only for the current widget ID
	endIf
endEvent

function SetWidgetClientInfo(int a_widgetID, string a_clientString)
	Debug.Trace("SKI_WidgetManager: SetWidgetClientInfo(a_widgetID = " + a_widgetID + ", a_clientString = " + a_clientString + ")")
	UI.InvokeString(_hudMenu, "_root.WidgetContainer." + a_widgetID + ".setClientInfo", a_clientString)
endFunction

function SetWidgetPositionX(int a_widgetID, float a_x)
	Debug.Trace("SKI_WidgetManager: SetWidgetPositionX(a_widgetID = " + a_widgetID + ", a_x = " + a_x + ")")
	UI.InvokeNumber(_hudMenu, "_root.WidgetContainer." + a_widgetID + ".setPositionX", a_x)
endFunction

function SetWidgetPositionY(int a_widgetID, float a_y)
	Debug.Trace("SKI_WidgetManager: SetWidgetPositionY(a_widgetID = " + a_widgetID + ", a_y = " + a_y + ")")
	UI.InvokeNumber(_hudMenu, "_root.WidgetContainer." + a_widgetID + ".setPositionY", a_y)
endFunction

function SetWidgetAlpha(int a_widgetID, int a_alpha)
	Debug.Trace("SKI_WidgetManager: SetWidgetAlpha(a_widgetID = " + a_widgetID + ", a_alpha = " + a_alpha + ")")
	UI.InvokeNumber(_hudMenu, "_root.WidgetContainer." + a_widgetID + ".setAlpha", a_alpha)
endFunction

function SetWidgetModes(int a_widgetID, string[] a_modes)
	Debug.Trace("SKI_WidgetManager: SetWidgetModes(a_widgetID = " + a_widgetID + ", a_modes = " + a_modes + ")")
	UI.InvokeStringA(_hudMenu, "_root.WidgetContainer." + a_widgetID + ".setModes", a_modes)
endFunction
