#include "GameEvents.h"

// For testing
//EventDispatcher<TESSleepStartEvent>* g_sleepStartEventDispatcher = (EventDispatcher<TESSleepStartEvent>*) 0x012DAC10;
EventDispatcher<InputEvent,InputEvent*> ** g_inputEventDispatcher = (EventDispatcher<InputEvent,InputEvent*>**) 0x13A4124;