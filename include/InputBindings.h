#ifndef PHX_InputBindings
#define PHX_InputBindings
#define __FFI_IGNORE__

#include "Common.h"
#include "Vec2.h"

PHX_API void           InputBindings_Init                    ();
PHX_API void           InputBindings_Free                    ();

PHX_API void           InputBindings_Update                  ();

//PHX_API void           InputBindings_Register                (cstr name, InputBinding* bindings, int count);
//PHX_API void           InputBindings_Unregister              (cstr name, InputBinding* bindings, int count);

PHX_API bool           InputBinding_GetPressed              (InputBinding*);
PHX_API bool           InputBinding_GetDown                 (InputBinding*);
PHX_API bool           InputBinding_GetReleased             (InputBinding*);
PHX_API float          InputBinding_GetValue                (InputBinding*);
PHX_API Vec2f          InputBinding_GetVecValue             (InputBinding*);

PHX_API float          InputBinding_GetXValue               (InputBinding*);
PHX_API float          InputBinding_GetYValue               (InputBinding*);

PHX_API bool           InputBinding_GetXPosPressed          (InputBinding*);
PHX_API bool           InputBinding_GetXPosDown             (InputBinding*);
PHX_API bool           InputBinding_GetXPosReleased         (InputBinding*);
PHX_API bool           InputBinding_GetXNegPressed          (InputBinding*);
PHX_API bool           InputBinding_GetXNegDown             (InputBinding*);
PHX_API bool           InputBinding_GetXNegReleased         (InputBinding*);
PHX_API bool           InputBinding_GetYPosPressed          (InputBinding*);
PHX_API bool           InputBinding_GetYPosDown             (InputBinding*);
PHX_API bool           InputBinding_GetYPosReleased         (InputBinding*);
PHX_API bool           InputBinding_GetYNegPressed          (InputBinding*);
PHX_API bool           InputBinding_GetYNegDown             (InputBinding*);
PHX_API bool           InputBinding_GetYNegReleased         (InputBinding*);

/* --- Convenience API ------------------------------------------------------ */

PHX_API const float    InputBindings_DefaultPressThreshold;
PHX_API const float    InputBindings_DefaultReleaseThreshold;
PHX_API const float    InputBindings_DefaultExponent;
PHX_API const float    InputBindings_DefaultDeadzone;
PHX_API const float    InputBindings_DefaultMinValue;
PHX_API const float    InputBindings_DefaultMaxValue;

//PHX_API InputBinding*  InputBinding_CreateButton             (Button);
//PHX_API InputBinding*  InputBinding_CreateButtonFromAxis     (Button);
//PHX_API InputBinding*  InputBinding_CreateAxis               (Button);
//PHX_API InputBinding*  InputBinding_CreateAxisFromButtons    (Button pos, Button neg);
//PHX_API InputBinding*  InputBinding_CreateAxis2DFromAxes     (Button x, Button y);
//PHX_API InputBinding*  InputBinding_CreateAxis2DFromButtons  (Button xPos, Button xNeg, Button yPos, Button yNeg);
//PHX_API InputBinding*  InputBinding_Clone                    (InputBinding*);
//PHX_API void           InputBinding_Free                     (InputBinding*);

//PHX_API void           InputBinding_SetActive                (InputBinding*, bool);

/* --- Convenience API  (Fluent Interface) ---------------------------------- */

PHX_API InputBinding*  InputBinding_SetDeadzone              (InputBinding*, float);
PHX_API InputBinding*  InputBinding_SetExponent              (InputBinding*, float);
PHX_API InputBinding*  InputBinding_SetInvertX               (InputBinding*, bool);
PHX_API InputBinding*  InputBinding_SetInvertY               (InputBinding*, bool);
PHX_API InputBinding*  InputBinding_SetRange                 (InputBinding*, float min, float max);
PHX_API InputBinding*  InputBinding_SetThresholds            (InputBinding*, float press, float release);

#endif
