Scriptname Scaleform Hidden

Function SetBool(string menu, string target, bool value) global native
Function SetNumber(string menu, string target, float value) global native
Function SetString(string menu, string target, string value) global native

bool Function GetBool(string menu, string target) global native
float Function GetNumber(string menu, string target) global native
string Function GetString(string menu, string target) global native

Function Invoke(string menu, string target) global native
Function InvokeBool(string menu, string target, bool arg) global native
Function InvokeNumber(string menu, string target, float arg) global native
Function InvokeString(string menu, string target, string arg) global native

Function IsMenuOpen(string menu) global native
