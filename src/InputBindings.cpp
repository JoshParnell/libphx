#include "Array.h"
#include "ArrayList.h"
#include "Input.h"
#include "InputBindings.h"
#include "InputEvent.h"
#include "Lua.h"
#include "PhxMath.h"
#include "PhxString.h"
#include "State.h"

static const int BindCount = 4;

struct RawButton {
  Button button;
  float  value;
};

struct AggregateButton {
  State  state;
  LuaRef onPressed;
  LuaRef onDown;
  LuaRef onReleased;
};

struct AggregateAxis {
  float  value;
  bool   invert;
  LuaRef onChanged;
};

struct AggregateAxis2D {
  Vec2f  value;
  LuaRef onChanged;
};

struct InputBinding {
  cstr            name;
  RawButton       rawButtons[4][BindCount];

  float           pressThreshold;
  float           releaseThreshold;
  float           exponent;
  float           deadzone;
  float           minValue;
  float           maxValue;

  Lua*            luaInstance;
  AggregateButton buttons[4];
  AggregateAxis   axes[2];
  AggregateAxis2D axis2D;
};

struct DownBinding {
  InputBinding*    binding;
  AggregateButton* button;
};

struct InputBindings {
  //ArrayList(InputBinding, inactiveBindings);
  ArrayList(InputBinding, activeBindings);
  ArrayList(DownBinding,  downBindings);
} static self = { 0 };


void InputBindings_Init () {
  //ArrayList_Reserve(self.inactiveBindings, 64);
  ArrayList_Reserve(self.activeBindings,   64);
  ArrayList_Reserve(self.downBindings,      8);
}

void InputBindings_Free () {
  //ArrayList_Free(self.inactiveBindings);
  ArrayList_Free(self.activeBindings);
  ArrayList_Free(self.downBindings);
}

static void InputBindings_RaiseCallback (cstr event, InputBinding* binding, LuaRef callback) {
  printf("%s - %s\n", event, binding->name);
  UNUSED(callback);
  /* TODO : Decide what all we want to pass to the callbacks (values, states, ...?) */
  //if (callback) {
  //  Lua_PushRef(binding->luaInstance, callback);
  //  Lua_PushPtr(binding->luaInstance, binding);
  //  Lua_Call(binding->luaInstance, 1, 0, 0);
  //}
}

void InputBindings_UpdateBinding (InputBinding* binding) {
  Vec2f value = {};
  float* axisValues[2] = { &value.x, &value.y };


  //Update Value
  {
    for (int iAxis = 0; iAxis < Array_GetSize(binding->axes); iAxis++) {
      float* axisValue = axisValues[iAxis];
      for (int iBind = 0; iBind < BindCount; iBind++) {
        *axisValue += binding->rawButtons[2*iAxis + 0][iBind].value;
        *axisValue -= binding->rawButtons[2*iAxis + 1][iBind].value;
      }
      *axisValue = (*axisValue - binding->deadzone) / (1.0f - binding->deadzone);
      *axisValue = Pow(*axisValue, binding->exponent);
      *axisValue = Clamp(*axisValue, binding->minValue, binding->maxValue);
    }

    float len = Vec2f_Length(value);
    if (len > 1.0f)
      Vec2f_IMuls(&value, 1.0f / len);
  }


  //Axis2D
  {
    AggregateAxis2D* axis2D = &binding->axis2D;
    if (!Vec2f_Equal(value, axis2D->value)) {
      axis2D->value = value;
      InputBindings_RaiseCallback("Changed", binding, axis2D->onChanged);
    }
  }


  //Axes
  {
    for (int iAxis = 0; iAxis < Array_GetSize(binding->axes); iAxis++) {
      AggregateAxis* axis = &binding->axes[iAxis];
      if (*axisValues[iAxis] != axis->value) {
        axis->value = *axisValues[iAxis];
        InputBindings_RaiseCallback(iAxis == 0 ? "Changed X" : "Changed Y", binding, axis->onChanged);
      }
    }
  }


  //Buttons
  for (int iBtn = 0; iBtn < Array_GetSize(binding->buttons); iBtn++) {
    AggregateButton* button = &binding->buttons[iBtn];
    float axisValue = binding->axes[iBtn / 2].value;
    bool isPos = !(iBtn & 1);

    //Pressed
    if (!HAS_FLAG(button->state, State_Down)) {
      if (isPos ? axisValue > binding->pressThreshold : axisValue < -binding->pressThreshold) {
        button->state |= State_Pressed;
        button->state |= State_Down;
        InputBindings_RaiseCallback("Pressed", binding, button->onPressed);

        DownBinding downBinding = {};
        downBinding.binding     = binding;
        downBinding.button      = button;
        ArrayList_Append(self.downBindings, downBinding);
      }
    //Released
    } else {
      if (isPos ? axisValue < binding->releaseThreshold : axisValue > -binding->releaseThreshold) {
        button->state |= State_Released;
        button->state &= ~State_Down;
        InputBindings_RaiseCallback("Released", binding, button->onReleased);

        ArrayList_RemoveWhere(self.downBindings, DownBinding,
          x->binding == binding && x->button == button
        );
      }
    }
  }
}

void InputBindings_Update () {
  //Down
  ArrayList_ForEachI(self.downBindings, i) {
    DownBinding down = ArrayList_Get(self.downBindings, i);
    InputBindings_RaiseCallback("Down", down.binding, down.button->onDown);
  }

  InputEvent event = {};
  while (Input_GetNextEvent(&event)) {
    //Match
    ArrayList_ForEachReverse(self.activeBindings, InputBinding, binding) {
      for (int iBtn = 0; iBtn < Array_GetSize(binding->rawButtons); iBtn++) {
        for (int iBind = 0; iBind < Array_GetSize(binding->rawButtons[iBtn]); iBind++) {
          RawButton* button = &binding->rawButtons[iBtn][iBind];

          if (event.button == button->button) {
            button->value = event.value;
            InputBindings_UpdateBinding(binding);
          }
        }
      }
    }
  }
}

#if false
void InputBindings_Register (InputBinding* binding) {
  //Lua* lua = Lua_GetActive();
  //if (!lua)
  //  Fatal("InputBinding_Register: No Lua instance is active");

  InputBinding binding = {};
  binding.states[Idx_xPos].button = binding->xPos;
  binding.states[Idx_xNeg].button = binding->xNeg;
  binding.states[Idx_yPos].button = binding->yPos;
  binding.states[Idx_yNeg].button = binding->yNeg;
  binding.pressThreshold          = binding->pressThreshold;
  binding.releaseThreshold        = binding->releaseThreshold;
  binding.exponent                = binding->exponent;

  //registeredBinding.luaInstance = lua;
  //registeredBinding.onPressed   = Lua_GetRef(lua);
  //registeredBinding.onDown      = Lua_GetRef(lua);
  //registeredBinding.onReleased  = Lua_GetRef(lua);
  //registeredBinding.onChanged   = Lua_GetRef(lua);

  ArrayList_Append(self.activeBindings, binding);
}

void InputBindings_Unregister (InputBinding* binding) {
  ArrayList_ForEachIReverse(self.downBindings, i) {
    RegisteredBinding* downBinding = ArrayList_Get(self.downBindings, i);
    if (StrEqual(binding->name, downBinding->name)) {
      ArrayList_RemoveAt(self.downBindings, i);
    }
  }

  ArrayList_ForEachIReverse(self.activeBindings, i) {
    RegisteredBinding* binding2 = ArrayList_GetPtr(self.activeBindings, i);
    if (StrEqual(binding->name, binding2->name)) {
      //Lua_ReleaseRef(binding2->luaInstance, binding2->onPressed);
      //Lua_ReleaseRef(binding2->luaInstance, binding2->onDown);
      //Lua_ReleaseRef(binding2->luaInstance, binding2->onReleased);
      //Lua_ReleaseRef(binding2->luaInstance, binding2->onChanged);
      ArrayList_RemoveAt(self.activeBindings, i);
    }
  }
}

const float InputBindings_DefaultPressThreshold   =  0.5f;
const float InputBindings_DefaultReleaseThreshold =  0.3f;
const float InputBindings_DefaultExponent         =  1.0f;
const float InputBindings_DefaultDeadzone         =  0.2f;
const float InputBindings_DefaultMinValue         = -FLT_MAX;
const float InputBindings_DefaultMaxValue         =  FLT_MAX;

void InputBindings_RegisterAll (InputBinding* binding, int count) {
  for (int i = 0; i < count; i++)
    InputBindings_Register(&binding[i]);
}

void InputBindings_UnregisterAll (InputBinding* binding, int count) {
  for (int i = 0; i < count; i++)
    InputBindings_Unregister(&binding[i]);
}
#endif

static const int iXPos = 0;
static const int iXNeg = 1;
static const int iYPos = 2;
static const int iYNeg = 3;

static const int iX    = 0;
static const int iY    = 1;

inline static bool InputBinding_GetButtonState (InputBinding* binding, int iBtn, State state) {
  return HAS_FLAG(binding->buttons[iBtn].state, state);
}

bool InputBinding_GetPressed (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXPos, State_Pressed);
}

bool InputBinding_GetDown (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXPos, State_Down);
}

bool InputBinding_GetReleased (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXPos, State_Released);
}

float InputBinding_GetValue (InputBinding* binding) {
  return binding->axes[iX].value;
}

Vec2f InputBinding_GetVecValue (InputBinding* binding) {
  return binding->axis2D.value;
}

float InputBinding_GetXValue (InputBinding* binding) {
  return binding->axes[iX].value;
}

float InputBinding_GetYValue (InputBinding* binding) {
  return binding->axes[iY].value;
}

bool InputBinding_GetXPosPressed (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXPos, State_Pressed);
}

bool InputBinding_GetXPosDown (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXPos, State_Down);
}

bool InputBinding_GetXPosReleased (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXPos, State_Released);
}

bool InputBinding_GetXNegPressed (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXNeg, State_Pressed);
}

bool InputBinding_GetXNegDown (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXNeg, State_Down);
}

bool InputBinding_GetXNegReleased (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iXNeg, State_Released);
}

bool InputBinding_GetYPosPressed (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iYPos, State_Pressed);
}

bool InputBinding_GetYPosDown (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iYPos, State_Down);
}

bool InputBinding_GetYPosReleased (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iYPos, State_Released);
}

bool InputBinding_GetYNegPressed (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iYNeg, State_Pressed);
}

bool InputBinding_GetYNegDown (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iYNeg, State_Down);
}

bool InputBinding_GetYNegReleased (InputBinding* binding) {
  return InputBinding_GetButtonState(binding, iYNeg, State_Released);
}

InputBinding* InputBinding_SetDeadzone (InputBinding* binding, float deadzone) {
  binding->deadzone = deadzone;
  return binding;
}

InputBinding* InputBinding_SetExponent (InputBinding* binding, float exponent) {
  binding->exponent = exponent;
  return binding;
}

inline static void InputBinding_SetInvert (InputBinding* binding, int iAxis, bool invert) {
  AggregateAxis* axis = &binding->axes[iAxis];
  if (invert != axis->invert) {
    axis->invert = invert;

    for (int iBind = 0; iBind < BindCount; iBind++) {
      RawButton* btnPos = &binding->rawButtons[2*iAxis + 0][iBind];
      RawButton* btnNeg = &binding->rawButtons[2*iAxis + 1][iBind];

      Button temp    = btnPos->button;
      btnPos->button = btnNeg->button;
      btnNeg->button = temp;
    }
  }
}

InputBinding* InputBinding_SetInvertX (InputBinding* binding, bool invert) {
  InputBinding_SetInvert(binding, 0, invert);
  return binding;
}

InputBinding* InputBinding_SetInvertY (InputBinding* binding, bool invert) {
  InputBinding_SetInvert(binding, 1, invert);
  return binding;
}

InputBinding* InputBinding_SetRange (InputBinding* binding, float min, float max) {
  binding->minValue = min;
  binding->maxValue = max;
  return binding;
}

InputBinding* InputBinding_SetThresholds (InputBinding* binding, float press, float release) {
  binding->pressThreshold   = press;
  binding->releaseThreshold = release;
  return binding;
}


/* TODO : Probably easier to aggregate values to point to top level Vec2f
 *        instead of juggling indicies and shit. */
/* TODO : What happens if you change binding settings while down? */
