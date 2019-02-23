#include "Array.h"
#include "Button.h"
#include "DeviceType.h"

#include <stdio.h>

const Button Button_Null                 = 0;
const Button Button_First                = 1;

const Button Button_Keyboard_First       = Button_First;
const Button Button_Keyboard_A           = Button_Keyboard_First + 0;
const Button Button_Keyboard_B           = Button_Keyboard_First + 1;
const Button Button_Keyboard_C           = Button_Keyboard_First + 2;
const Button Button_Keyboard_D           = Button_Keyboard_First + 3;
const Button Button_Keyboard_E           = Button_Keyboard_First + 4;
const Button Button_Keyboard_F           = Button_Keyboard_First + 5;
const Button Button_Keyboard_G           = Button_Keyboard_First + 6;
const Button Button_Keyboard_H           = Button_Keyboard_First + 7;
const Button Button_Keyboard_I           = Button_Keyboard_First + 8;
const Button Button_Keyboard_J           = Button_Keyboard_First + 9;
const Button Button_Keyboard_K           = Button_Keyboard_First + 10;
const Button Button_Keyboard_L           = Button_Keyboard_First + 11;
const Button Button_Keyboard_M           = Button_Keyboard_First + 12;
const Button Button_Keyboard_N           = Button_Keyboard_First + 13;
const Button Button_Keyboard_O           = Button_Keyboard_First + 14;
const Button Button_Keyboard_P           = Button_Keyboard_First + 15;
const Button Button_Keyboard_Q           = Button_Keyboard_First + 16;
const Button Button_Keyboard_R           = Button_Keyboard_First + 17;
const Button Button_Keyboard_S           = Button_Keyboard_First + 18;
const Button Button_Keyboard_T           = Button_Keyboard_First + 19;
const Button Button_Keyboard_U           = Button_Keyboard_First + 20;
const Button Button_Keyboard_V           = Button_Keyboard_First + 21;
const Button Button_Keyboard_W           = Button_Keyboard_First + 22;
const Button Button_Keyboard_X           = Button_Keyboard_First + 23;
const Button Button_Keyboard_Y           = Button_Keyboard_First + 24;
const Button Button_Keyboard_Z           = Button_Keyboard_First + 25;
const Button Button_Keyboard_N0          = Button_Keyboard_First + 26;
const Button Button_Keyboard_N1          = Button_Keyboard_First + 27;
const Button Button_Keyboard_N2          = Button_Keyboard_First + 28;
const Button Button_Keyboard_N3          = Button_Keyboard_First + 29;
const Button Button_Keyboard_N4          = Button_Keyboard_First + 30;
const Button Button_Keyboard_N5          = Button_Keyboard_First + 31;
const Button Button_Keyboard_N6          = Button_Keyboard_First + 32;
const Button Button_Keyboard_N7          = Button_Keyboard_First + 33;
const Button Button_Keyboard_N8          = Button_Keyboard_First + 34;
const Button Button_Keyboard_N9          = Button_Keyboard_First + 35;
const Button Button_Keyboard_F1          = Button_Keyboard_First + 36;
const Button Button_Keyboard_F2          = Button_Keyboard_First + 37;
const Button Button_Keyboard_F3          = Button_Keyboard_First + 38;
const Button Button_Keyboard_F4          = Button_Keyboard_First + 39;
const Button Button_Keyboard_F5          = Button_Keyboard_First + 40;
const Button Button_Keyboard_F6          = Button_Keyboard_First + 41;
const Button Button_Keyboard_F7          = Button_Keyboard_First + 42;
const Button Button_Keyboard_F8          = Button_Keyboard_First + 43;
const Button Button_Keyboard_F9          = Button_Keyboard_First + 44;
const Button Button_Keyboard_F10         = Button_Keyboard_First + 45;
const Button Button_Keyboard_F11         = Button_Keyboard_First + 46;
const Button Button_Keyboard_F12         = Button_Keyboard_First + 47;
const Button Button_Keyboard_F13         = Button_Keyboard_First + 48;
const Button Button_Keyboard_F14         = Button_Keyboard_First + 49;
const Button Button_Keyboard_F15         = Button_Keyboard_First + 50;
const Button Button_Keyboard_F16         = Button_Keyboard_First + 51;
const Button Button_Keyboard_F17         = Button_Keyboard_First + 52;
const Button Button_Keyboard_F18         = Button_Keyboard_First + 53;
const Button Button_Keyboard_F19         = Button_Keyboard_First + 54;
const Button Button_Keyboard_F20         = Button_Keyboard_First + 55;
const Button Button_Keyboard_F21         = Button_Keyboard_First + 56;
const Button Button_Keyboard_F22         = Button_Keyboard_First + 57;
const Button Button_Keyboard_F23         = Button_Keyboard_First + 58;
const Button Button_Keyboard_F24         = Button_Keyboard_First + 59;
const Button Button_Keyboard_KP0         = Button_Keyboard_First + 60;
const Button Button_Keyboard_KP1         = Button_Keyboard_First + 61;
const Button Button_Keyboard_KP2         = Button_Keyboard_First + 62;
const Button Button_Keyboard_KP3         = Button_Keyboard_First + 63;
const Button Button_Keyboard_KP4         = Button_Keyboard_First + 64;
const Button Button_Keyboard_KP5         = Button_Keyboard_First + 65;
const Button Button_Keyboard_KP6         = Button_Keyboard_First + 66;
const Button Button_Keyboard_KP7         = Button_Keyboard_First + 67;
const Button Button_Keyboard_KP8         = Button_Keyboard_First + 68;
const Button Button_Keyboard_KP9         = Button_Keyboard_First + 69;
const Button Button_Keyboard_KPNumLock   = Button_Keyboard_First + 70;
const Button Button_Keyboard_KPDivide    = Button_Keyboard_First + 71;
const Button Button_Keyboard_KPMultiply  = Button_Keyboard_First + 72;
const Button Button_Keyboard_KPSubtract  = Button_Keyboard_First + 73;
const Button Button_Keyboard_KPAdd       = Button_Keyboard_First + 74;
const Button Button_Keyboard_KPEnter     = Button_Keyboard_First + 75;
const Button Button_Keyboard_KPDecimal   = Button_Keyboard_First + 76;
const Button Button_Keyboard_Backspace   = Button_Keyboard_First + 77;
const Button Button_Keyboard_Escape      = Button_Keyboard_First + 78;
const Button Button_Keyboard_Return      = Button_Keyboard_First + 79;
const Button Button_Keyboard_Space       = Button_Keyboard_First + 80;
const Button Button_Keyboard_Tab         = Button_Keyboard_First + 81;
const Button Button_Keyboard_Backtick    = Button_Keyboard_First + 82;
const Button Button_Keyboard_CapsLock    = Button_Keyboard_First + 83;
const Button Button_Keyboard_Minus       = Button_Keyboard_First + 84;
const Button Button_Keyboard_Equals      = Button_Keyboard_First + 85;
const Button Button_Keyboard_LBracket    = Button_Keyboard_First + 86;
const Button Button_Keyboard_RBracket    = Button_Keyboard_First + 87;
const Button Button_Keyboard_Backslash   = Button_Keyboard_First + 88;
const Button Button_Keyboard_Semicolon   = Button_Keyboard_First + 89;
const Button Button_Keyboard_Apostrophe  = Button_Keyboard_First + 90;
const Button Button_Keyboard_Comma       = Button_Keyboard_First + 91;
const Button Button_Keyboard_Period      = Button_Keyboard_First + 92;
const Button Button_Keyboard_Slash       = Button_Keyboard_First + 93;
const Button Button_Keyboard_PrintScreen = Button_Keyboard_First + 94;
const Button Button_Keyboard_ScrollLock  = Button_Keyboard_First + 95;
const Button Button_Keyboard_Pause       = Button_Keyboard_First + 96;
const Button Button_Keyboard_Insert      = Button_Keyboard_First + 97;
const Button Button_Keyboard_Delete      = Button_Keyboard_First + 98;
const Button Button_Keyboard_Home        = Button_Keyboard_First + 99;
const Button Button_Keyboard_End         = Button_Keyboard_First + 100;
const Button Button_Keyboard_PageUp      = Button_Keyboard_First + 101;
const Button Button_Keyboard_PageDown    = Button_Keyboard_First + 102;
const Button Button_Keyboard_Right       = Button_Keyboard_First + 103;
const Button Button_Keyboard_Left        = Button_Keyboard_First + 104;
const Button Button_Keyboard_Down        = Button_Keyboard_First + 105;
const Button Button_Keyboard_Up          = Button_Keyboard_First + 106;
const Button Button_Keyboard_LCtrl       = Button_Keyboard_First + 107;
const Button Button_Keyboard_LShift      = Button_Keyboard_First + 108;
const Button Button_Keyboard_LAlt        = Button_Keyboard_First + 109;
const Button Button_Keyboard_LMeta       = Button_Keyboard_First + 110;
const Button Button_Keyboard_RCtrl       = Button_Keyboard_First + 111;
const Button Button_Keyboard_RShift      = Button_Keyboard_First + 112;
const Button Button_Keyboard_RAlt        = Button_Keyboard_First + 113;
const Button Button_Keyboard_RMeta       = Button_Keyboard_First + 114;
const Button Button_Keyboard_Last        = Button_Keyboard_First + 115;

const Button Button_Mouse_First          = Button_Keyboard_Last + 1;
const Button Button_Mouse_Left           = Button_Mouse_First + 0;
const Button Button_Mouse_Middle         = Button_Mouse_First + 1;
const Button Button_Mouse_Right          = Button_Mouse_First + 2;
const Button Button_Mouse_X1             = Button_Mouse_First + 3;
const Button Button_Mouse_X2             = Button_Mouse_First + 4;
const Button Button_Mouse_X              = Button_Mouse_First + 5;
const Button Button_Mouse_Y              = Button_Mouse_First + 6;
const Button Button_Mouse_ScrollX        = Button_Mouse_First + 7;
const Button Button_Mouse_ScrollY        = Button_Mouse_First + 8;
const Button Button_Mouse_Last           = Button_Mouse_First + 8;

const Button Button_Gamepad_First        = Button_Mouse_Last + 1;
const Button Button_Gamepad_Button_First = Button_Gamepad_First + 0;
const Button Button_Gamepad_A            = Button_Gamepad_First + 0;
const Button Button_Gamepad_B            = Button_Gamepad_First + 1;
const Button Button_Gamepad_X            = Button_Gamepad_First + 2;
const Button Button_Gamepad_Y            = Button_Gamepad_First + 3;
const Button Button_Gamepad_Back         = Button_Gamepad_First + 4;
const Button Button_Gamepad_Guide        = Button_Gamepad_First + 5;
const Button Button_Gamepad_Start        = Button_Gamepad_First + 6;
const Button Button_Gamepad_LStick       = Button_Gamepad_First + 7;
const Button Button_Gamepad_RStick       = Button_Gamepad_First + 8;
const Button Button_Gamepad_LBumper      = Button_Gamepad_First + 9;
const Button Button_Gamepad_RBumper      = Button_Gamepad_First + 10;
const Button Button_Gamepad_Up           = Button_Gamepad_First + 11;
const Button Button_Gamepad_Down         = Button_Gamepad_First + 12;
const Button Button_Gamepad_Left         = Button_Gamepad_First + 13;
const Button Button_Gamepad_Right        = Button_Gamepad_First + 14;
const Button Button_Gamepad_Button_Last  = Button_Gamepad_First + 14;
const Button Button_Gamepad_Axis_First   = Button_Gamepad_First + 15;
const Button Button_Gamepad_LTrigger     = Button_Gamepad_First + 15;
const Button Button_Gamepad_RTrigger     = Button_Gamepad_First + 16;
const Button Button_Gamepad_LStickX      = Button_Gamepad_First + 17;
const Button Button_Gamepad_LStickY      = Button_Gamepad_First + 18;
const Button Button_Gamepad_RStickX      = Button_Gamepad_First + 19;
const Button Button_Gamepad_RStickY      = Button_Gamepad_First + 20;
const Button Button_Gamepad_Axis_Last    = Button_Gamepad_First + 20;
const Button Button_Gamepad_Last         = Button_Gamepad_First + 20;

const Button Button_System_First         = Button_Gamepad_Last + 1;
const Button Button_System_Exit          = Button_System_First + 0;
const Button Button_System_Last          = Button_System_First + 0;

const Button Button_Last                 = Button_System_Last;

DeviceType Button_ToDeviceType (Button button) {
       if (button == Button_Null)          return DeviceType_Null;
  else if (button <= Button_Keyboard_Last) return DeviceType_Keyboard;
  else if (button <= Button_Mouse_Last)    return DeviceType_Mouse;
  else if (button <= Button_Gamepad_Last)  return DeviceType_Gamepad;
  else if (button <= Button_System_Last)   return DeviceType_Null;
  else {
    Fatal("Button_ToDeviceType: Unknown Button: %i", button);
    return DeviceType_Null;
  }
}

cstr Button_ToString (Button button) {
  switch (button) {
    default:
      static char buffer[512];
      snprintf(buffer, (size_t) Array_GetSize(buffer), "Unknown (%i)", button);
      return buffer;

      case Button_Null:                 return "Button_Null";
      case Button_Keyboard_A:           return "Button_Keyboard_A";
      case Button_Keyboard_B:           return "Button_Keyboard_B";
      case Button_Keyboard_C:           return "Button_Keyboard_C";
      case Button_Keyboard_D:           return "Button_Keyboard_D";
      case Button_Keyboard_E:           return "Button_Keyboard_E";
      case Button_Keyboard_F:           return "Button_Keyboard_F";
      case Button_Keyboard_G:           return "Button_Keyboard_G";
      case Button_Keyboard_H:           return "Button_Keyboard_H";
      case Button_Keyboard_I:           return "Button_Keyboard_I";
      case Button_Keyboard_J:           return "Button_Keyboard_J";
      case Button_Keyboard_K:           return "Button_Keyboard_K";
      case Button_Keyboard_L:           return "Button_Keyboard_L";
      case Button_Keyboard_M:           return "Button_Keyboard_M";
      case Button_Keyboard_N:           return "Button_Keyboard_N";
      case Button_Keyboard_O:           return "Button_Keyboard_O";
      case Button_Keyboard_P:           return "Button_Keyboard_P";
      case Button_Keyboard_Q:           return "Button_Keyboard_Q";
      case Button_Keyboard_R:           return "Button_Keyboard_R";
      case Button_Keyboard_S:           return "Button_Keyboard_S";
      case Button_Keyboard_T:           return "Button_Keyboard_T";
      case Button_Keyboard_U:           return "Button_Keyboard_U";
      case Button_Keyboard_V:           return "Button_Keyboard_V";
      case Button_Keyboard_W:           return "Button_Keyboard_W";
      case Button_Keyboard_X:           return "Button_Keyboard_X";
      case Button_Keyboard_Y:           return "Button_Keyboard_Y";
      case Button_Keyboard_Z:           return "Button_Keyboard_Z";
      case Button_Keyboard_N0:          return "Button_Keyboard_N0";
      case Button_Keyboard_N1:          return "Button_Keyboard_N1";
      case Button_Keyboard_N2:          return "Button_Keyboard_N2";
      case Button_Keyboard_N3:          return "Button_Keyboard_N3";
      case Button_Keyboard_N4:          return "Button_Keyboard_N4";
      case Button_Keyboard_N5:          return "Button_Keyboard_N5";
      case Button_Keyboard_N6:          return "Button_Keyboard_N6";
      case Button_Keyboard_N7:          return "Button_Keyboard_N7";
      case Button_Keyboard_N8:          return "Button_Keyboard_N8";
      case Button_Keyboard_N9:          return "Button_Keyboard_N9";
      case Button_Keyboard_F1:          return "Button_Keyboard_F1";
      case Button_Keyboard_F2:          return "Button_Keyboard_F2";
      case Button_Keyboard_F3:          return "Button_Keyboard_F3";
      case Button_Keyboard_F4:          return "Button_Keyboard_F4";
      case Button_Keyboard_F5:          return "Button_Keyboard_F5";
      case Button_Keyboard_F6:          return "Button_Keyboard_F6";
      case Button_Keyboard_F7:          return "Button_Keyboard_F7";
      case Button_Keyboard_F8:          return "Button_Keyboard_F8";
      case Button_Keyboard_F9:          return "Button_Keyboard_F9";
      case Button_Keyboard_F10:         return "Button_Keyboard_F10";
      case Button_Keyboard_F11:         return "Button_Keyboard_F11";
      case Button_Keyboard_F12:         return "Button_Keyboard_F12";
      case Button_Keyboard_F13:         return "Button_Keyboard_F13";
      case Button_Keyboard_F14:         return "Button_Keyboard_F14";
      case Button_Keyboard_F15:         return "Button_Keyboard_F15";
      case Button_Keyboard_F16:         return "Button_Keyboard_F16";
      case Button_Keyboard_F17:         return "Button_Keyboard_F17";
      case Button_Keyboard_F18:         return "Button_Keyboard_F18";
      case Button_Keyboard_F19:         return "Button_Keyboard_F19";
      case Button_Keyboard_F20:         return "Button_Keyboard_F20";
      case Button_Keyboard_F21:         return "Button_Keyboard_F21";
      case Button_Keyboard_F22:         return "Button_Keyboard_F22";
      case Button_Keyboard_F23:         return "Button_Keyboard_F23";
      case Button_Keyboard_F24:         return "Button_Keyboard_F24";
      case Button_Keyboard_KP0:         return "Button_Keyboard_KP0";
      case Button_Keyboard_KP1:         return "Button_Keyboard_KP1";
      case Button_Keyboard_KP2:         return "Button_Keyboard_KP2";
      case Button_Keyboard_KP3:         return "Button_Keyboard_KP3";
      case Button_Keyboard_KP4:         return "Button_Keyboard_KP4";
      case Button_Keyboard_KP5:         return "Button_Keyboard_KP5";
      case Button_Keyboard_KP6:         return "Button_Keyboard_KP6";
      case Button_Keyboard_KP7:         return "Button_Keyboard_KP7";
      case Button_Keyboard_KP8:         return "Button_Keyboard_KP8";
      case Button_Keyboard_KP9:         return "Button_Keyboard_KP9";
      case Button_Keyboard_KPNumLock:   return "Button_Keyboard_KPNumLock";
      case Button_Keyboard_KPDivide:    return "Button_Keyboard_KPDivide";
      case Button_Keyboard_KPMultiply:  return "Button_Keyboard_KPMultiply";
      case Button_Keyboard_KPSubtract:  return "Button_Keyboard_KPSubtract";
      case Button_Keyboard_KPAdd:       return "Button_Keyboard_KPAdd";
      case Button_Keyboard_KPEnter:     return "Button_Keyboard_KPEnter";
      case Button_Keyboard_KPDecimal:   return "Button_Keyboard_KPDecimal";
      case Button_Keyboard_Backspace:   return "Button_Keyboard_Backspace";
      case Button_Keyboard_Escape:      return "Button_Keyboard_Escape";
      case Button_Keyboard_Return:      return "Button_Keyboard_Return";
      case Button_Keyboard_Space:       return "Button_Keyboard_Space";
      case Button_Keyboard_Tab:         return "Button_Keyboard_Tab";
      case Button_Keyboard_Backtick:    return "Button_Keyboard_Backtick";
      case Button_Keyboard_CapsLock:    return "Button_Keyboard_CapsLock";
      case Button_Keyboard_Minus:       return "Button_Keyboard_Minus";
      case Button_Keyboard_Equals:      return "Button_Keyboard_Equals";
      case Button_Keyboard_LBracket:    return "Button_Keyboard_LBracket";
      case Button_Keyboard_RBracket:    return "Button_Keyboard_RBracket";
      case Button_Keyboard_Backslash:   return "Button_Keyboard_Backslash";
      case Button_Keyboard_Semicolon:   return "Button_Keyboard_Semicolon";
      case Button_Keyboard_Apostrophe:  return "Button_Keyboard_Apostrophe";
      case Button_Keyboard_Comma:       return "Button_Keyboard_Comma";
      case Button_Keyboard_Period:      return "Button_Keyboard_Period";
      case Button_Keyboard_Slash:       return "Button_Keyboard_Slash";
      case Button_Keyboard_PrintScreen: return "Button_Keyboard_PrintScreen";
      case Button_Keyboard_ScrollLock:  return "Button_Keyboard_ScrollLock";
      case Button_Keyboard_Pause:       return "Button_Keyboard_Pause";
      case Button_Keyboard_Insert:      return "Button_Keyboard_Insert";
      case Button_Keyboard_Delete:      return "Button_Keyboard_Delete";
      case Button_Keyboard_Home:        return "Button_Keyboard_Home";
      case Button_Keyboard_End:         return "Button_Keyboard_End";
      case Button_Keyboard_PageUp:      return "Button_Keyboard_PageUp";
      case Button_Keyboard_PageDown:    return "Button_Keyboard_PageDown";
      case Button_Keyboard_Right:       return "Button_Keyboard_Right";
      case Button_Keyboard_Left:        return "Button_Keyboard_Left";
      case Button_Keyboard_Down:        return "Button_Keyboard_Down";
      case Button_Keyboard_Up:          return "Button_Keyboard_Up";
      case Button_Keyboard_LCtrl:       return "Button_Keyboard_LCtrl";
      case Button_Keyboard_LShift:      return "Button_Keyboard_LShift";
      case Button_Keyboard_LAlt:        return "Button_Keyboard_LAlt";
      case Button_Keyboard_LMeta:       return "Button_Keyboard_LMeta";
      case Button_Keyboard_RCtrl:       return "Button_Keyboard_RCtrl";
      case Button_Keyboard_RShift:      return "Button_Keyboard_RShift";
      case Button_Keyboard_RAlt:        return "Button_Keyboard_RAlt";
      case Button_Keyboard_RMeta:       return "Button_Keyboard_RMeta";
      case Button_Mouse_Left:           return "Button_Mouse_Left";
      case Button_Mouse_Middle:         return "Button_Mouse_Middle";
      case Button_Mouse_Right:          return "Button_Mouse_Right";
      case Button_Mouse_X1:             return "Button_Mouse_X1";
      case Button_Mouse_X2:             return "Button_Mouse_X2";
      case Button_Mouse_X:              return "Button_Mouse_X";
      case Button_Mouse_Y:              return "Button_Mouse_Y";
      case Button_Mouse_ScrollX:        return "Button_Mouse_ScrollX";
      case Button_Mouse_ScrollY:        return "Button_Mouse_ScrollY";
      case Button_Gamepad_A:            return "Button_Gamepad_A";
      case Button_Gamepad_B:            return "Button_Gamepad_B";
      case Button_Gamepad_X:            return "Button_Gamepad_X";
      case Button_Gamepad_Y:            return "Button_Gamepad_Y";
      case Button_Gamepad_Back:         return "Button_Gamepad_Back";
      case Button_Gamepad_Guide:        return "Button_Gamepad_Guide";
      case Button_Gamepad_Start:        return "Button_Gamepad_Start";
      case Button_Gamepad_LStick:       return "Button_Gamepad_LStick";
      case Button_Gamepad_RStick:       return "Button_Gamepad_RStick";
      case Button_Gamepad_LBumper:      return "Button_Gamepad_LBumper";
      case Button_Gamepad_RBumper:      return "Button_Gamepad_RBumper";
      case Button_Gamepad_Up:           return "Button_Gamepad_Up";
      case Button_Gamepad_Down:         return "Button_Gamepad_Down";
      case Button_Gamepad_Left:         return "Button_Gamepad_Left";
      case Button_Gamepad_Right:        return "Button_Gamepad_Right";
      case Button_Gamepad_LTrigger:     return "Button_Gamepad_LTrigger";
      case Button_Gamepad_RTrigger:     return "Button_Gamepad_RTrigger";
      case Button_Gamepad_LStickX:      return "Button_Gamepad_LStickX";
      case Button_Gamepad_LStickY:      return "Button_Gamepad_LStickY";
      case Button_Gamepad_RStickX:      return "Button_Gamepad_RStickX";
      case Button_Gamepad_RStickY:      return "Button_Gamepad_RStickY";
      case Button_System_Exit:          return "Button_System_Exit";
  }
}

bool Button_IsAutoRelease (Button button) {
  switch (button) {
    default:
      return false;

    case Button_Mouse_ScrollX:
    case Button_Mouse_ScrollY:
    case Button_System_Exit:
      return true;
  }
}

Button Button_FromSDLScancode (SDL_Scancode scancode) {
  switch(scancode) {
    /* NOTE : We don't support every possible scan code. */
    default: return Button_Null;

    case SDL_SCANCODE_UNKNOWN:      return Button_Null;
    case SDL_SCANCODE_A:            return Button_Keyboard_A;
    case SDL_SCANCODE_B:            return Button_Keyboard_B;
    case SDL_SCANCODE_C:            return Button_Keyboard_C;
    case SDL_SCANCODE_D:            return Button_Keyboard_D;
    case SDL_SCANCODE_E:            return Button_Keyboard_E;
    case SDL_SCANCODE_F:            return Button_Keyboard_F;
    case SDL_SCANCODE_G:            return Button_Keyboard_G;
    case SDL_SCANCODE_H:            return Button_Keyboard_H;
    case SDL_SCANCODE_I:            return Button_Keyboard_I;
    case SDL_SCANCODE_J:            return Button_Keyboard_J;
    case SDL_SCANCODE_K:            return Button_Keyboard_K;
    case SDL_SCANCODE_L:            return Button_Keyboard_L;
    case SDL_SCANCODE_M:            return Button_Keyboard_M;
    case SDL_SCANCODE_N:            return Button_Keyboard_N;
    case SDL_SCANCODE_O:            return Button_Keyboard_O;
    case SDL_SCANCODE_P:            return Button_Keyboard_P;
    case SDL_SCANCODE_Q:            return Button_Keyboard_Q;
    case SDL_SCANCODE_R:            return Button_Keyboard_R;
    case SDL_SCANCODE_S:            return Button_Keyboard_S;
    case SDL_SCANCODE_T:            return Button_Keyboard_T;
    case SDL_SCANCODE_U:            return Button_Keyboard_U;
    case SDL_SCANCODE_V:            return Button_Keyboard_V;
    case SDL_SCANCODE_W:            return Button_Keyboard_W;
    case SDL_SCANCODE_X:            return Button_Keyboard_X;
    case SDL_SCANCODE_Y:            return Button_Keyboard_Y;
    case SDL_SCANCODE_Z:            return Button_Keyboard_Z;
    case SDL_SCANCODE_0:            return Button_Keyboard_N0;
    case SDL_SCANCODE_1:            return Button_Keyboard_N1;
    case SDL_SCANCODE_2:            return Button_Keyboard_N2;
    case SDL_SCANCODE_3:            return Button_Keyboard_N3;
    case SDL_SCANCODE_4:            return Button_Keyboard_N4;
    case SDL_SCANCODE_5:            return Button_Keyboard_N5;
    case SDL_SCANCODE_6:            return Button_Keyboard_N6;
    case SDL_SCANCODE_7:            return Button_Keyboard_N7;
    case SDL_SCANCODE_8:            return Button_Keyboard_N8;
    case SDL_SCANCODE_9:            return Button_Keyboard_N9;
    case SDL_SCANCODE_F1:           return Button_Keyboard_F1;
    case SDL_SCANCODE_F2:           return Button_Keyboard_F2;
    case SDL_SCANCODE_F3:           return Button_Keyboard_F3;
    case SDL_SCANCODE_F4:           return Button_Keyboard_F4;
    case SDL_SCANCODE_F5:           return Button_Keyboard_F5;
    case SDL_SCANCODE_F6:           return Button_Keyboard_F6;
    case SDL_SCANCODE_F7:           return Button_Keyboard_F7;
    case SDL_SCANCODE_F8:           return Button_Keyboard_F8;
    case SDL_SCANCODE_F9:           return Button_Keyboard_F9;
    case SDL_SCANCODE_F10:          return Button_Keyboard_F10;
    case SDL_SCANCODE_F11:          return Button_Keyboard_F11;
    case SDL_SCANCODE_F12:          return Button_Keyboard_F12;
    case SDL_SCANCODE_F13:          return Button_Keyboard_F13;
    case SDL_SCANCODE_F14:          return Button_Keyboard_F14;
    case SDL_SCANCODE_F15:          return Button_Keyboard_F15;
    case SDL_SCANCODE_F16:          return Button_Keyboard_F16;
    case SDL_SCANCODE_F17:          return Button_Keyboard_F17;
    case SDL_SCANCODE_F18:          return Button_Keyboard_F18;
    case SDL_SCANCODE_F19:          return Button_Keyboard_F19;
    case SDL_SCANCODE_F20:          return Button_Keyboard_F20;
    case SDL_SCANCODE_F21:          return Button_Keyboard_F21;
    case SDL_SCANCODE_F22:          return Button_Keyboard_F22;
    case SDL_SCANCODE_F23:          return Button_Keyboard_F23;
    case SDL_SCANCODE_F24:          return Button_Keyboard_F24;
    case SDL_SCANCODE_KP_0:         return Button_Keyboard_KP0;
    case SDL_SCANCODE_KP_1:         return Button_Keyboard_KP1;
    case SDL_SCANCODE_KP_2:         return Button_Keyboard_KP2;
    case SDL_SCANCODE_KP_3:         return Button_Keyboard_KP3;
    case SDL_SCANCODE_KP_4:         return Button_Keyboard_KP4;
    case SDL_SCANCODE_KP_5:         return Button_Keyboard_KP5;
    case SDL_SCANCODE_KP_6:         return Button_Keyboard_KP6;
    case SDL_SCANCODE_KP_7:         return Button_Keyboard_KP7;
    case SDL_SCANCODE_KP_8:         return Button_Keyboard_KP8;
    case SDL_SCANCODE_KP_9:         return Button_Keyboard_KP9;
    case SDL_SCANCODE_NUMLOCKCLEAR: return Button_Keyboard_KPNumLock;
    case SDL_SCANCODE_KP_DIVIDE:    return Button_Keyboard_KPDivide;
    case SDL_SCANCODE_KP_MULTIPLY:  return Button_Keyboard_KPMultiply;
    case SDL_SCANCODE_KP_MINUS:     return Button_Keyboard_KPSubtract;
    case SDL_SCANCODE_KP_PLUS:      return Button_Keyboard_KPAdd;
    case SDL_SCANCODE_KP_ENTER:     return Button_Keyboard_KPEnter;
    case SDL_SCANCODE_KP_DECIMAL:   return Button_Keyboard_KPDecimal;
    case SDL_SCANCODE_BACKSPACE:    return Button_Keyboard_Backspace;
    case SDL_SCANCODE_ESCAPE:       return Button_Keyboard_Escape;
    case SDL_SCANCODE_RETURN:       return Button_Keyboard_Return;
    case SDL_SCANCODE_SPACE:        return Button_Keyboard_Space;
    case SDL_SCANCODE_TAB:          return Button_Keyboard_Tab;
    case SDL_SCANCODE_GRAVE:        return Button_Keyboard_Backtick;
    case SDL_SCANCODE_CAPSLOCK:     return Button_Keyboard_CapsLock;
    case SDL_SCANCODE_MINUS:        return Button_Keyboard_Minus;
    case SDL_SCANCODE_EQUALS:       return Button_Keyboard_Equals;
    case SDL_SCANCODE_LEFTBRACKET:  return Button_Keyboard_LBracket;
    case SDL_SCANCODE_RIGHTBRACKET: return Button_Keyboard_RBracket;
    case SDL_SCANCODE_BACKSLASH:    return Button_Keyboard_Backslash;
    case SDL_SCANCODE_SEMICOLON:    return Button_Keyboard_Semicolon;
    case SDL_SCANCODE_APOSTROPHE:   return Button_Keyboard_Apostrophe;
    case SDL_SCANCODE_COMMA:        return Button_Keyboard_Comma;
    case SDL_SCANCODE_PERIOD:       return Button_Keyboard_Period;
    case SDL_SCANCODE_SLASH:        return Button_Keyboard_Slash;
    case SDL_SCANCODE_PRINTSCREEN:  return Button_Keyboard_PrintScreen;
    case SDL_SCANCODE_SCROLLLOCK:   return Button_Keyboard_ScrollLock;
    case SDL_SCANCODE_PAUSE:        return Button_Keyboard_Pause;
    case SDL_SCANCODE_INSERT:       return Button_Keyboard_Insert;
    case SDL_SCANCODE_DELETE:       return Button_Keyboard_Delete;
    case SDL_SCANCODE_HOME:         return Button_Keyboard_Home;
    case SDL_SCANCODE_END:          return Button_Keyboard_End;
    case SDL_SCANCODE_PAGEUP:       return Button_Keyboard_PageUp;
    case SDL_SCANCODE_PAGEDOWN:     return Button_Keyboard_PageDown;
    case SDL_SCANCODE_RIGHT:        return Button_Keyboard_Right;
    case SDL_SCANCODE_LEFT:         return Button_Keyboard_Left;
    case SDL_SCANCODE_DOWN:         return Button_Keyboard_Down;
    case SDL_SCANCODE_UP:           return Button_Keyboard_Up;
    case SDL_SCANCODE_LCTRL:        return Button_Keyboard_LCtrl;
    case SDL_SCANCODE_LSHIFT:       return Button_Keyboard_LShift;
    case SDL_SCANCODE_LALT:         return Button_Keyboard_LAlt;
    case SDL_SCANCODE_LGUI:         return Button_Keyboard_LMeta;
    case SDL_SCANCODE_RCTRL:        return Button_Keyboard_RCtrl;
    case SDL_SCANCODE_RSHIFT:       return Button_Keyboard_RShift;
    case SDL_SCANCODE_RALT:         return Button_Keyboard_RAlt;
    case SDL_SCANCODE_RGUI:         return Button_Keyboard_RMeta;
  }
}

SDL_Scancode Button_ToSDLScancode (Button button) {
  switch(button) {
    default: Fatal("Button_ToSDLScancode: Unhandled case: %i", button);

    case Button_Null:                 return SDL_SCANCODE_UNKNOWN;
    case Button_Keyboard_A:           return SDL_SCANCODE_A;
    case Button_Keyboard_B:           return SDL_SCANCODE_B;
    case Button_Keyboard_C:           return SDL_SCANCODE_C;
    case Button_Keyboard_D:           return SDL_SCANCODE_D;
    case Button_Keyboard_E:           return SDL_SCANCODE_E;
    case Button_Keyboard_F:           return SDL_SCANCODE_F;
    case Button_Keyboard_G:           return SDL_SCANCODE_G;
    case Button_Keyboard_H:           return SDL_SCANCODE_H;
    case Button_Keyboard_I:           return SDL_SCANCODE_I;
    case Button_Keyboard_J:           return SDL_SCANCODE_J;
    case Button_Keyboard_K:           return SDL_SCANCODE_K;
    case Button_Keyboard_L:           return SDL_SCANCODE_L;
    case Button_Keyboard_M:           return SDL_SCANCODE_M;
    case Button_Keyboard_N:           return SDL_SCANCODE_N;
    case Button_Keyboard_O:           return SDL_SCANCODE_O;
    case Button_Keyboard_P:           return SDL_SCANCODE_P;
    case Button_Keyboard_Q:           return SDL_SCANCODE_Q;
    case Button_Keyboard_R:           return SDL_SCANCODE_R;
    case Button_Keyboard_S:           return SDL_SCANCODE_S;
    case Button_Keyboard_T:           return SDL_SCANCODE_T;
    case Button_Keyboard_U:           return SDL_SCANCODE_U;
    case Button_Keyboard_V:           return SDL_SCANCODE_V;
    case Button_Keyboard_W:           return SDL_SCANCODE_W;
    case Button_Keyboard_X:           return SDL_SCANCODE_X;
    case Button_Keyboard_Y:           return SDL_SCANCODE_Y;
    case Button_Keyboard_Z:           return SDL_SCANCODE_Z;
    case Button_Keyboard_N0:          return SDL_SCANCODE_0;
    case Button_Keyboard_N1:          return SDL_SCANCODE_1;
    case Button_Keyboard_N2:          return SDL_SCANCODE_2;
    case Button_Keyboard_N3:          return SDL_SCANCODE_3;
    case Button_Keyboard_N4:          return SDL_SCANCODE_4;
    case Button_Keyboard_N5:          return SDL_SCANCODE_5;
    case Button_Keyboard_N6:          return SDL_SCANCODE_6;
    case Button_Keyboard_N7:          return SDL_SCANCODE_7;
    case Button_Keyboard_N8:          return SDL_SCANCODE_8;
    case Button_Keyboard_N9:          return SDL_SCANCODE_9;
    case Button_Keyboard_F1:          return SDL_SCANCODE_F1;
    case Button_Keyboard_F2:          return SDL_SCANCODE_F2;
    case Button_Keyboard_F3:          return SDL_SCANCODE_F3;
    case Button_Keyboard_F4:          return SDL_SCANCODE_F4;
    case Button_Keyboard_F5:          return SDL_SCANCODE_F5;
    case Button_Keyboard_F6:          return SDL_SCANCODE_F6;
    case Button_Keyboard_F7:          return SDL_SCANCODE_F7;
    case Button_Keyboard_F8:          return SDL_SCANCODE_F8;
    case Button_Keyboard_F9:          return SDL_SCANCODE_F9;
    case Button_Keyboard_F10:         return SDL_SCANCODE_F10;
    case Button_Keyboard_F11:         return SDL_SCANCODE_F11;
    case Button_Keyboard_F12:         return SDL_SCANCODE_F12;
    case Button_Keyboard_F13:         return SDL_SCANCODE_F13;
    case Button_Keyboard_F14:         return SDL_SCANCODE_F14;
    case Button_Keyboard_F15:         return SDL_SCANCODE_F15;
    case Button_Keyboard_F16:         return SDL_SCANCODE_F16;
    case Button_Keyboard_F17:         return SDL_SCANCODE_F17;
    case Button_Keyboard_F18:         return SDL_SCANCODE_F18;
    case Button_Keyboard_F19:         return SDL_SCANCODE_F19;
    case Button_Keyboard_F20:         return SDL_SCANCODE_F20;
    case Button_Keyboard_F21:         return SDL_SCANCODE_F21;
    case Button_Keyboard_F22:         return SDL_SCANCODE_F22;
    case Button_Keyboard_F23:         return SDL_SCANCODE_F23;
    case Button_Keyboard_F24:         return SDL_SCANCODE_F24;
    case Button_Keyboard_KP0:         return SDL_SCANCODE_KP_0;
    case Button_Keyboard_KP1:         return SDL_SCANCODE_KP_1;
    case Button_Keyboard_KP2:         return SDL_SCANCODE_KP_2;
    case Button_Keyboard_KP3:         return SDL_SCANCODE_KP_3;
    case Button_Keyboard_KP4:         return SDL_SCANCODE_KP_4;
    case Button_Keyboard_KP5:         return SDL_SCANCODE_KP_5;
    case Button_Keyboard_KP6:         return SDL_SCANCODE_KP_6;
    case Button_Keyboard_KP7:         return SDL_SCANCODE_KP_7;
    case Button_Keyboard_KP8:         return SDL_SCANCODE_KP_8;
    case Button_Keyboard_KP9:         return SDL_SCANCODE_KP_9;
    case Button_Keyboard_KPNumLock:   return SDL_SCANCODE_NUMLOCKCLEAR;
    case Button_Keyboard_KPDivide:    return SDL_SCANCODE_KP_DIVIDE;
    case Button_Keyboard_KPMultiply:  return SDL_SCANCODE_KP_MULTIPLY;
    case Button_Keyboard_KPSubtract:  return SDL_SCANCODE_KP_MINUS;
    case Button_Keyboard_KPAdd:       return SDL_SCANCODE_KP_PLUS;
    case Button_Keyboard_KPEnter:     return SDL_SCANCODE_KP_ENTER;
    case Button_Keyboard_KPDecimal:   return SDL_SCANCODE_KP_DECIMAL;
    case Button_Keyboard_Backspace:   return SDL_SCANCODE_BACKSPACE;
    case Button_Keyboard_Escape:      return SDL_SCANCODE_ESCAPE;
    case Button_Keyboard_Return:      return SDL_SCANCODE_RETURN;
    case Button_Keyboard_Space:       return SDL_SCANCODE_SPACE;
    case Button_Keyboard_Tab:         return SDL_SCANCODE_TAB;
    case Button_Keyboard_Backtick:    return SDL_SCANCODE_GRAVE;
    case Button_Keyboard_CapsLock:    return SDL_SCANCODE_CAPSLOCK;
    case Button_Keyboard_Minus:       return SDL_SCANCODE_MINUS;
    case Button_Keyboard_Equals:      return SDL_SCANCODE_EQUALS;
    case Button_Keyboard_LBracket:    return SDL_SCANCODE_LEFTBRACKET;
    case Button_Keyboard_RBracket:    return SDL_SCANCODE_RIGHTBRACKET;
    case Button_Keyboard_Backslash:   return SDL_SCANCODE_BACKSLASH;
    case Button_Keyboard_Semicolon:   return SDL_SCANCODE_SEMICOLON;
    case Button_Keyboard_Apostrophe:  return SDL_SCANCODE_APOSTROPHE;
    case Button_Keyboard_Comma:       return SDL_SCANCODE_COMMA;
    case Button_Keyboard_Period:      return SDL_SCANCODE_PERIOD;
    case Button_Keyboard_Slash:       return SDL_SCANCODE_SLASH;
    case Button_Keyboard_PrintScreen: return SDL_SCANCODE_PRINTSCREEN;
    case Button_Keyboard_ScrollLock:  return SDL_SCANCODE_SCROLLLOCK;
    case Button_Keyboard_Pause:       return SDL_SCANCODE_PAUSE;
    case Button_Keyboard_Insert:      return SDL_SCANCODE_INSERT;
    case Button_Keyboard_Delete:      return SDL_SCANCODE_DELETE;
    case Button_Keyboard_Home:        return SDL_SCANCODE_HOME;
    case Button_Keyboard_End:         return SDL_SCANCODE_END;
    case Button_Keyboard_PageUp:      return SDL_SCANCODE_PAGEUP;
    case Button_Keyboard_PageDown:    return SDL_SCANCODE_PAGEDOWN;
    case Button_Keyboard_Right:       return SDL_SCANCODE_RIGHT;
    case Button_Keyboard_Left:        return SDL_SCANCODE_LEFT;
    case Button_Keyboard_Down:        return SDL_SCANCODE_DOWN;
    case Button_Keyboard_Up:          return SDL_SCANCODE_UP;
    case Button_Keyboard_LCtrl:       return SDL_SCANCODE_LCTRL;
    case Button_Keyboard_LShift:      return SDL_SCANCODE_LSHIFT;
    case Button_Keyboard_LAlt:        return SDL_SCANCODE_LALT;
    case Button_Keyboard_LMeta:       return SDL_SCANCODE_LGUI;
    case Button_Keyboard_RCtrl:       return SDL_SCANCODE_RCTRL;
    case Button_Keyboard_RShift:      return SDL_SCANCODE_RSHIFT;
    case Button_Keyboard_RAlt:        return SDL_SCANCODE_RALT;
    case Button_Keyboard_RMeta:       return SDL_SCANCODE_RGUI;
  }
}

Button Button_FromSDLMouseButton (uint8 mouseButton) {
  switch (mouseButton) {
    default: Fatal("Button_FromSDLMouseButton: Unhandled case: %i", mouseButton);

    case SDL_BUTTON_LEFT:   return Button_Mouse_Left;
    case SDL_BUTTON_MIDDLE: return Button_Mouse_Middle;
    case SDL_BUTTON_RIGHT:  return Button_Mouse_Right;
    case SDL_BUTTON_X1:     return Button_Mouse_X1;
    case SDL_BUTTON_X2:     return Button_Mouse_X2;
  }
}

uint8 Button_ToSDLMouseButton (Button button) {
  switch (button) {
    default: Fatal("Button_ToSDLMouseButton: Unhandled case: %i", button);

    case Button_Mouse_Left:   return SDL_BUTTON_LEFT;
    case Button_Mouse_Middle: return SDL_BUTTON_MIDDLE;
    case Button_Mouse_Right:  return SDL_BUTTON_RIGHT;
    case Button_Mouse_X1:     return SDL_BUTTON_X1;
    case Button_Mouse_X2:     return SDL_BUTTON_X2;
  }
}

Button Button_FromSDLControllerAxis (SDL_GameControllerAxis controllerAxis) {
  switch (controllerAxis) {
    default: Fatal("Button_FromSDLControllerAxis: Unhandled case: %i", controllerAxis);

    case SDL_CONTROLLER_AXIS_LEFTX:        return Button_Gamepad_LStickX;
    case SDL_CONTROLLER_AXIS_LEFTY:        return Button_Gamepad_LStickY;
    case SDL_CONTROLLER_AXIS_RIGHTX:       return Button_Gamepad_RStickX;
    case SDL_CONTROLLER_AXIS_RIGHTY:       return Button_Gamepad_RStickY;
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:  return Button_Gamepad_LTrigger;
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return Button_Gamepad_RTrigger;
  }
}

SDL_GameControllerAxis Button_ToSDLControllerAxis (Button button) {
  switch (button) {
    default: Fatal("Button_ToSDLControllerAxis: Unhandled case: %i", button);

    case Button_Gamepad_LStickX:  return SDL_CONTROLLER_AXIS_LEFTX;
    case Button_Gamepad_LStickY:  return SDL_CONTROLLER_AXIS_LEFTY;
    case Button_Gamepad_RStickX:  return SDL_CONTROLLER_AXIS_RIGHTX;
    case Button_Gamepad_RStickY:  return SDL_CONTROLLER_AXIS_RIGHTY;
    case Button_Gamepad_LTrigger: return SDL_CONTROLLER_AXIS_TRIGGERLEFT;
    case Button_Gamepad_RTrigger: return SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
  }
}

Button Button_FromSDLControllerButton (SDL_GameControllerButton controllerButton) {
  switch (controllerButton) {
    default: Fatal("Button_FromSDLControllerButton: Unhandled case: %i", controllerButton);

    case SDL_CONTROLLER_BUTTON_A:             return Button_Gamepad_A;
    case SDL_CONTROLLER_BUTTON_B:             return Button_Gamepad_B;
    case SDL_CONTROLLER_BUTTON_X:             return Button_Gamepad_X;
    case SDL_CONTROLLER_BUTTON_Y:             return Button_Gamepad_Y;
    case SDL_CONTROLLER_BUTTON_BACK:          return Button_Gamepad_Back;
    case SDL_CONTROLLER_BUTTON_GUIDE:         return Button_Gamepad_Guide;
    case SDL_CONTROLLER_BUTTON_START:         return Button_Gamepad_Start;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:     return Button_Gamepad_LStick;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:    return Button_Gamepad_RStick;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return Button_Gamepad_LBumper;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return Button_Gamepad_RBumper;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:       return Button_Gamepad_Up;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:     return Button_Gamepad_Down;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:     return Button_Gamepad_Left;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:    return Button_Gamepad_Right;
  }
}

SDL_GameControllerButton Button_ToSDLControllerButton (Button button) {
  switch (button) {
    default: Fatal("Button_ToSDLControllerButton: Unhandled case: %i", button);

    case Button_Gamepad_A:       return SDL_CONTROLLER_BUTTON_A;
    case Button_Gamepad_B:       return SDL_CONTROLLER_BUTTON_B;
    case Button_Gamepad_X:       return SDL_CONTROLLER_BUTTON_X;
    case Button_Gamepad_Y:       return SDL_CONTROLLER_BUTTON_Y;
    case Button_Gamepad_Back:    return SDL_CONTROLLER_BUTTON_BACK;
    case Button_Gamepad_Guide:   return SDL_CONTROLLER_BUTTON_GUIDE;
    case Button_Gamepad_Start:   return SDL_CONTROLLER_BUTTON_START;
    case Button_Gamepad_LStick:  return SDL_CONTROLLER_BUTTON_LEFTSTICK;
    case Button_Gamepad_RStick:  return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    case Button_Gamepad_LBumper: return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    case Button_Gamepad_RBumper: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    case Button_Gamepad_Up:      return SDL_CONTROLLER_BUTTON_DPAD_UP;
    case Button_Gamepad_Down:    return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    case Button_Gamepad_Left:    return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    case Button_Gamepad_Right:   return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
  }
}
