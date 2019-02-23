/* TODO : Currently disabled due to matrix transition. There's a lot of matrix
 *        code in here that will be a hassle to rewrite. */

#if 0
#include "Matrix.h"
#include "PhxMemory.h"
#include "Tex2D.h"
#include "VR.h"
#include "openvr/openvr.h"

#if WINDOWS
  #pragma comment(lib, "openvr_api.lib")
#endif

/* TODO : Make conditionally available. */

const VRDeviceType VRDeviceType_Invalid = vr::TrackedDeviceClass_Invalid;
const VRDeviceType VRDeviceType_HMD = vr::TrackedDeviceClass_HMD;
const VRDeviceType VRDeviceType_Controller = vr::TrackedDeviceClass_Controller;
const VRDeviceType VRDeviceType_Tracker = vr::TrackedDeviceClass_TrackingReference;
const VRDeviceType VRDeviceType_Other = vr::TrackedDeviceClass_Other;

static vr::IVRSystem* vrSystem = 0;
static vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];

typedef vr::HmdMatrix34_t Matrix3x4;
typedef vr::HmdMatrix44_t Matrix4x4;
typedef vr::VRControllerState_t ControllerState;

bool VR_Exists () {
  static bool checked = false;
  static bool present;
  if (checked)
    return present;
  else {
    present = vr::VR_IsHmdPresent();
    checked = true;
    return present;
  }
}

bool VR_Init () {
  vr::HmdError err;
  vrSystem = vr::VR_Init(&err, vr::VRApplication_Scene);
  if (vrSystem)
    return true;
  else
    return false;
}

void VR_Free () {
  if (vrSystem)
    vr::VR_Shutdown();
}

Vec2i VR_GetResolution () {
  Vec2i res;
  vrSystem->GetRecommendedRenderTargetSize((uint32*)&res.x, (uint32*)&res.y);
  return res;
}

Matrix VR_GetProjection (int eye, double z0, double z1) {
  Matrix4x4 matrix = vrSystem->GetProjectionMatrix(
    eye == 0 ? vr::Eye_Left : vr::Eye_Right,
    (float)z0, (float)z1, vr::API_OpenGL);

  return Matrix_Create(
    matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
    matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
    matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
    matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]);
}

Matrix VR_GetEyeTransform (int eye) {
  Matrix3x4 matrix = vrSystem->GetEyeToHeadTransform(
    eye == 0 ? vr::Eye_Left : vr::Eye_Right);

  return Matrix_Create(
    matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0,
    matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0,
    matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0,
    matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1);
}

int VR_GetMaxAxis () {
  return vr::k_unControllerStateAxisCount;
}

int VR_GetMaxButton () {
  return vr::k_EButton_Max;
}

int VR_GetMaxDevice () {
  return vr::k_unMaxTrackedDeviceCount;
}

inline static cstr GetStringProperty (int index, vr::TrackedDeviceProperty prop) {
  vr::TrackedPropertyError error;
  uint32 buffSize = vrSystem->GetStringTrackedDeviceProperty(
    index, prop, 0, 0, &error);
  char* result = (char*)MemAlloc(buffSize);
  vrSystem->GetStringTrackedDeviceProperty(index, prop, result, buffSize, &error);
  return result;
}

cstr VR_GetDeviceName (int index) {
  return GetStringProperty(index, vr::Prop_TrackingSystemName_String);
}

Matrix VR_GetDeviceTransform (int index) {
  Matrix3x4 matrix = poses[index].mDeviceToAbsoluteTracking;
  return Matrix_Create(
    matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0,
    matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0,
    matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0,
    matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1);
}

VRDeviceType VR_GetDeviceType (int index) {
  return (VRDeviceType)vrSystem->GetTrackedDeviceClass(index);
}

bool VR_IsDeviceConnected (int index) {
  return vrSystem->IsTrackedDeviceConnected(index);
}

struct VRController {
  int index;
  ControllerState state;
};

VRController* VR_OpenController (int index) {
  VRController* self = MemNew(VRController);
  self->index = index;
  vrSystem->GetControllerState(self->index, &self->state, sizeof(ControllerState));
  return self;
}

void VR_CloseController (VRController* self) {
  MemFree(self);
}

Vec2d VR_GetAxis (VRController* self, int axis) {
  vrSystem->GetControllerState(self->index, &self->state, sizeof(ControllerState));
  Vec2d result;
  result.x = self->state.rAxis[axis].x;
  result.y = self->state.rAxis[axis].y;
  return result;
}

bool VR_GetButton (VRController* self, int button) {
  vrSystem->GetControllerState(self->index, &self->state, sizeof(ControllerState));
  return (self->state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)button)) != 0;
}

void VR_Vibrate (VRController* self, int axis, int microseconds) {
  vrSystem->TriggerHapticPulse(self->index, axis, (unsigned short) microseconds);
}

void VR_GetTransforms () {
  vr::IVRCompositor* comp = vr::VRCompositor();
  comp->WaitGetPoses(poses, vr::k_unMaxTrackedDeviceCount, 0, 0);
}

void VR_Render (int eye, Tex2D* texture) {
  vr::IVRCompositor* comp = vr::VRCompositor();
  comp->CompositorBringToFront();
  vr::Texture_t vrt;
  vrt.handle = (void*)(size_t)Tex2D_GetHandle(texture);
  vrt.eType = vr::API_OpenGL;
  vrt.eColorSpace = vr::ColorSpace_Gamma;
  comp->Submit(eye == 0 ? vr::Eye_Left : vr::Eye_Right, &vrt);
}

#endif
