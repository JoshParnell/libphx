#define __FFI_IGNORE__
#if 0
#ifndef PHX_VR
#define PHX_VR

#include "Common.h"
#include "Vec2.h"

PHX_API const VRDeviceType VRDeviceType_Invalid;
PHX_API const VRDeviceType VRDeviceType_HMD;
PHX_API const VRDeviceType VRDeviceType_Controller;
PHX_API const VRDeviceType VRDeviceType_Tracker;
PHX_API const VRDeviceType VRDeviceType_Other;

PHX_API bool           VR_Exists              ();

PHX_API bool           VR_Init                ();
PHX_API void           VR_Free                (); 

PHX_API Vec2i          VR_GetResolution       ();
PHX_API Matrix         VR_GetProjection       (int eye, double z0, double z1);
PHX_API Matrix         VR_GetEyeTransform     (int eye);

PHX_API int            VR_GetMaxAxis          ();
PHX_API int            VR_GetMaxButton        ();
PHX_API int            VR_GetMaxDevice        ();
 
PHX_API cstr           VR_GetDeviceName       (int index);
PHX_API Matrix         VR_GetDeviceTransform  (int index);
PHX_API VRDeviceType   VR_GetDeviceType       (int index);
PHX_API bool           VR_IsDeviceConnected   (int index);

PHX_API VRController*  VR_OpenController      (int index);
PHX_API void           VR_CloseController     (VRController*);

PHX_API Vec2d          VR_GetAxis             (VRController*, int button);
PHX_API bool           VR_GetButton           (VRController*, int button);
PHX_API void           VR_Vibrate             (VRController*, int axis, int microseconds);

PHX_API void           VR_GetTransforms       ();
PHX_API void           VR_Render              (int eye, Tex2D* texture);
#endif

#endif
