#ifndef PHX_Midi
#define PHX_Midi

#include "Common.h"
#include "Vec2.h"

PHX_API int          MidiDevice_GetCount        ();
PHX_API MidiDevice*  MidiDevice_Open            (int index);
PHX_API void         MidiDevice_Close           (MidiDevice*);

PHX_API cstr         MidiDevice_GetNameByIndex  (int index);
PHX_API bool         MidiDevice_HasMessage      (MidiDevice*);
PHX_API Vec2i        MidiDevice_PopMessage      (MidiDevice*);

#endif
