#include "PhxMemory.h"
#include "Midi.h"
#include "PhxString.h"

#if WINDOWS
  #include <windows.h>
  #include <mmsystem.h>
#endif

#if LINUX
  #include <alsa/asoundlib.h>
#endif

#define MAX_QUEUED_MESSAGES 512

struct MidiDevice {
  int cursor;
  Vec2i buffer[MAX_QUEUED_MESSAGES];
  #if WINDOWS
    HMIDIIN handle;
  #endif
};

#if WINDOWS
inline static void MidiDevice_PushMessage (MidiDevice* self, int x, int y) {
  if (self->cursor < MAX_QUEUED_MESSAGES)
    self->buffer[self->cursor++] = Vec2i_Create(x, y);
}

void CALLBACK MidiDevice_Handler (
  HMIDIIN hMidiIn,
  UINT msg,
  DWORD_PTR instance,
  DWORD arg1,
  DWORD arg2)
{
  UNUSED(hMidiIn); UNUSED(arg2);
  uint8 status, byte1, byte2;
  MidiDevice* device = (MidiDevice*)instance;

  switch (msg) {
  case MIM_OPEN:
    /* Device Opened. */
    return;
  case MIM_CLOSE:
    /* Device Closed. */
    return;
  case MIM_DATA:
    status = (uint8)((arg1 >>  0) & 0x000000FF);
    byte1  = (uint8)((arg1 >>  8) & 0x000000FF);
    byte2  = (uint8)((arg1 >> 16) & 0x000000FF);

    /* CC. */
    if ((status & 0xF0) == 0xB0)
      MidiDevice_PushMessage(device, (int)byte1, (int)byte2);
    return;
  }
}

int MidiDevice_GetCount () {
  return midiInGetNumDevs();
}

MidiDevice* MidiDevice_Open (int index) {
  MidiDevice* self = MemNew(MidiDevice);
  self->cursor = 0;
  MMRESULT result;

  /* Open the port. */ {
    result = midiInOpen(
      &self->handle,
      index,
      (DWORD)(void*)MidiDevice_Handler,
      (DWORD)(void*)self,
      CALLBACK_FUNCTION);

    if (result != MMSYSERR_NOERROR)
      Fatal("MidiDevice_Open: failed to open device");
  }

  /* Reset timestamp & begin receiving. */ {
    result = midiInStart(self->handle);
    if (result != MMSYSERR_NOERROR)
      Fatal("MidiDevice_Open: failed to start device");
  }

  return self;
}

void MidiDevice_Close (MidiDevice* self) {
  midiInStop(self->handle);
  midiInClose(self->handle);
  MemFree(self);
}

cstr MidiDevice_GetNameByIndex (int index) {
  static char buffer[256] = { 0 };
  MIDIINCAPS caps;
  MMRESULT result = midiInGetDevCaps(index, &caps, sizeof(MIDIINCAPS));
  if (result != MMSYSERR_NOERROR)
    Fatal("MidiDevice_GetNameByIndex: failed to get device capabilities");
  #ifdef _UNICODE
    WideCharToMultiByte(CP_UTF8, 0, caps.szPname, -1, buffer, sizeof(buffer), 0, 0);
    buffer[sizeof(buffer) - 1] = 0;
    return buffer;
  #else
    return StrDup(caps.szPname);
  #endif
}

#else
  int MidiDevice_GetCount () {
    return 0;
  }

  MidiDevice* MidiDevice_Open (int) {
    return 0;
  }

  void MidiDevice_Close (MidiDevice*) {}

  cstr MidiDevice_GetNameByIndex (int) {
    return 0;
  }
#endif

bool MidiDevice_HasMessage (MidiDevice* self) {
  return self->cursor > 0;
}

Vec2i MidiDevice_PopMessage (MidiDevice* self) {
  if (self->cursor <= 0)
    Fatal("MidiDevice_PopMessage: device has no messages");
  self->cursor -= 1;
  return self->buffer[self->cursor];
}
