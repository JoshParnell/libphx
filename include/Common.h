#ifndef PHX_Common
#define PHX_Common

#include "PhxConfig.h"
#include "PhxInt.h"

#ifdef _WIN32
  #ifndef WINDOWS
    #define WINDOWS 1
  #endif
#elif __APPLE__
  #ifndef APPLE
    #define APPLE 1
    #define POSIX 1
  #endif
#elif __linux__
  #ifndef LINUX
    #define LINUX 1
    #define POSIX 1
  #endif
#endif

#if WINDOWS
  #if LIBPHX_BUILDING
    #define PHX_API           extern "C" __declspec(dllexport)
  #else
    #define PHX_API           extern "C" __declspec(dllimport)
  #endif

  #define DEBUG_BREAK         __debugbreak()
  #define PRIVATE
  #define ALIGN_OF(x)         __alignof(x)
  #define FORCE_INLINE        static __forceinline
  #define NO_ALIAS            __restrict
  #define IF_LIKELY(x)        if (x)
  #define IF_UNLIKELY(x)      if (x)

#else

  #define DEBUG_BREAK         __builtin_trap()
  #define PHX_API             extern "C" __attribute__((visibility("default")))
  #define PRIVATE             __attribute__((visibility("hidden")))
  #define ALIGN_OF(x)         __alignof__(x)
  #define FORCE_INLINE        inline static __attribute__((always_inline))
  #define NO_ALIAS            __restrict__
  #define IF_LIKELY(x)        if (__builtin_expect(!!(x), 1))
  #define IF_UNLIKELY(x)      if (__builtin_expect(!!(x), 0))

#endif

#define OPAQUE_T              struct
#define STRUCT_T              struct
#define ENUM_T
#define OFFSET_OF(st, m)      ((size_t)&(((st*)0)->m))
#define HAS_FLAG(x, flag)     ((x & flag) == flag)
#define UNUSED(x)             (void)x

/* --- Standard Aliases ----------------------------------------------------- */

  typedef unsigned long  ulong;
  typedef unsigned int   uint;
  typedef unsigned short ushort;
  typedef unsigned char  uchar;
  typedef char const*    cstr;

  typedef int8_t         int8;
  typedef int16_t        int16;
  typedef int32_t        int32;
  typedef int64_t        int64;

  typedef uint8_t        uint8;
  typedef uint16_t       uint16;
  typedef uint32_t       uint32;
  typedef uint64_t       uint64;

/* --- Opaque Types ------------------------------------------------------------
 *
 *   Note: The 'OPAQUE_T' and 'STRUCT_T' macros are used instead of struct as
 *         annotations for external tools to differentiate between struct
 *         forward declarations and opaque types.
 *
 * -------------------------------------------------------------------------- */

  OPAQUE_T BoxMesh;
  OPAQUE_T BoxTree;
  OPAQUE_T BSP;
  OPAQUE_T Bytes;
  OPAQUE_T Directory;
  OPAQUE_T File;
  OPAQUE_T Font;
  OPAQUE_T HashGrid;
  OPAQUE_T HashGridElem;
  OPAQUE_T HashMap;
  OPAQUE_T InputBinding;
  OPAQUE_T KDTree;
  OPAQUE_T LodMesh;
  OPAQUE_T MemPool;
  OPAQUE_T MemStack;
  OPAQUE_T Mesh;
  OPAQUE_T MidiDevice;
  OPAQUE_T Octree;
  OPAQUE_T Physics;
  OPAQUE_T RigidBody;
  OPAQUE_T RmGui;
  OPAQUE_T RNG;
  OPAQUE_T SDF;
  OPAQUE_T Shader;
  OPAQUE_T ShaderState;
  OPAQUE_T Socket;
  OPAQUE_T Sound;
  OPAQUE_T SoundDesc;
  OPAQUE_T StrBuffer;
  OPAQUE_T StrMap;
  OPAQUE_T StrMapIter;
  OPAQUE_T Tex1D;
  OPAQUE_T Tex2D;
  OPAQUE_T Tex3D;
  OPAQUE_T TexCube;
  OPAQUE_T Thread;
  OPAQUE_T ThreadPool;
  OPAQUE_T Timer;
  OPAQUE_T Trigger;
  OPAQUE_T Window;

/* --- Transparent Structs -------------------------------------------------- */

  STRUCT_T Box3i;
  STRUCT_T Box3d;
  STRUCT_T Box3f;
  STRUCT_T BSPNodeRef;
  STRUCT_T Collision;
  STRUCT_T Device;
  STRUCT_T IntersectSphereProfiling;
  STRUCT_T InputEvent;
  STRUCT_T LineSegment;
  STRUCT_T Matrix;
  STRUCT_T Plane;
  STRUCT_T Polygon;
  STRUCT_T Quat;
  STRUCT_T Ray;
  STRUCT_T RayCastResult;
  STRUCT_T ShapeCastResult;
  STRUCT_T Sphere;
  STRUCT_T Time;
  STRUCT_T Triangle;
  STRUCT_T TriangleTest;
  STRUCT_T Vec2i;
  STRUCT_T Vec2d;
  STRUCT_T Vec2f;
  STRUCT_T Vec3i;
  STRUCT_T Vec3d;
  STRUCT_T Vec3f;
  STRUCT_T Vec4i;
  STRUCT_T Vec4d;
  STRUCT_T Vec4f;
  STRUCT_T Vertex;

/* --- Enums ---------------------------------------------------------------- */

  ENUM_T typedef int32  BlendMode;
  ENUM_T typedef uint8  BSPNodeRel;
  ENUM_T typedef int32  Button;
  ENUM_T typedef int32  CollisionGroup;
  ENUM_T typedef int32  CollisionMask;
  ENUM_T typedef int32  CubeFace;
  ENUM_T typedef int32  CullFace;
  ENUM_T typedef int32  DataFormat;
  ENUM_T typedef int32  DeviceType;
  ENUM_T typedef uint32 Error;
  ENUM_T typedef int32  Metric;
  ENUM_T typedef int32  Modifier;
  ENUM_T typedef int32  PixelFormat;
  ENUM_T typedef uint8  PointClassification;
  ENUM_T typedef uint8  PolygonClassification;
  ENUM_T typedef int32  ResourceType;
  ENUM_T typedef int32  ShaderVarType;
  ENUM_T typedef int32  SocketType;
  ENUM_T typedef int32  State;
  ENUM_T typedef int32  TexFilter;
  ENUM_T typedef int32  TexFormat;
  ENUM_T typedef int32  TexWrapMode;
  ENUM_T typedef uint64 TimeStamp;
  ENUM_T typedef uint32 WindowMode;
  ENUM_T typedef int    WindowPos;

/* --- Deprecated ----------------------------------------------------------- */

  struct Gamepad;
  struct Joystick;
  typedef int32 GamepadAxis;
  typedef int32 GamepadButton;
  typedef int32 HatDir;
  typedef uchar Key;
  typedef int32 MouseButton;

/* --- Debugging ------------------------------------------------------------ */

/*  CHECK_LEVEL
 *    Determines the rigor with which internal engine code will check
 *    assertions and validate state. The granularity of CHECK_LEVEL allows for
 *    progressive debugging without having to immediately kill performance.
 *    Higher CHECK_LEVELs incurs higher performance penalties. The levels are
 *    defined qualitatively as follows:
 *
 *      0 : No checking. This is 'release'.
 *      1 : Light sanity checks that don't significantly affect performance.
 *          Assert et al. are defined. This is the recommended default for
 *          development.
 *      2 : Moderately-intrusive checks. Good for periodic internal validation.
 *      3 : Maximal rigor in checks, including those that incur heavy perf
 *          overheads. Intended only for debugging difficult bugs and for
 *          pre-release quality checks. */

#ifndef CHECK_LEVEL
  #define CHECK_LEVEL 0
#endif

#if CHECK_LEVEL == 0
  #define NCHECK(x) x
#else
  #define NCHECK(x)
#endif

#if CHECK_LEVEL >= 1
  #define  CHECK1(x) x
  #define  CHECK1PARAM(x) x,
  #define NCHECK1(x)
  #define Assert(x) _Assert((x) != 0, #x)
  #define Expect(x) _Expect((x) != 0, #x)
#else
  #define  CHECK1(x)
  #define  CHECK1PARAM(x)
  #define NCHECK1(x) x
  #define Expect(x)
  #define Assert(x)
#endif

#if CHECK_LEVEL >= 2
  #define  CHECK2(x) x
  #define  CHECK2PARAM(x) x,
  #define NCHECK2(x)
#else
  #define  CHECK2(x)
  #define  CHECK2PARAM(x)
  #define NCHECK2(x) x
#endif

#if CHECK_LEVEL >= 3
  #define  CHECK3(x) x
  #define  CHECK3PARAM(x) x,
  #define NCHECK3(x)
#else
  #define  CHECK3(x)
  #define  CHECK3PARAM(x)
  #define NCHECK3(x) x
#endif

/* Indicate fatal error and abort program. */
PHX_API void Fatal (cstr, ...);
inline void _Assert(bool condition, cstr expression) {
  if (!condition)
    Fatal("Assertion Failed!\n%s", expression);
}

/* Indicate non-fatal error and continue execution. */
PHX_API void Warn (cstr, ...);
inline void _Expect(bool condition, cstr expression) {
  if (!condition)
    Warn("Expect Failed!\n%s", expression);
}

#define NYI Fatal("NYI @ %s: %d", __FILE__, __LINE__);

template <class T>
inline static void Swap(T& a, T& b) {
  T temp = a;
  a = b;
  b = temp;
}

#endif

/* TODO : Export Assert and Expect for use in Lua */
