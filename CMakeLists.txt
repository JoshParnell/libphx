cmake_minimum_required (VERSION 3.0.2)
project (LibPHX)
set (CMAKE_BUILD_TYPE Release)

include_directories (include)
include_directories (ext)
include_directories (ext/bullet)

file (GLOB HEADERS "include/*.h")
file (GLOB SOURCES "src/*.cpp")

add_library (phx SHARED ${SOURCES} ${HEADERS})

target_compile_definitions (phx PRIVATE LIBPHX_BUILDING=1)

set_target_properties(phx PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
  set_target_properties(phx PROPERTIES OUTPUT_NAME "phx32")
else ()
  set_target_properties(phx PROPERTIES OUTPUT_NAME "phx64")
endif ()

if (WIN32)

  target_link_libraries (phx
    opengl32.lib
    glew32.lib
    user32.lib winmm.lib Ws2_32.lib
    SDL2.lib
    liblz4.lib
    freetype.lib
    lua51.lib)

  target_link_libraries (phx fmodL_vc.lib fmodstudioL_vc.lib)
  target_link_libraries (phx BulletCollision.lib BulletDynamics.lib LinearMath.lib)

  target_compile_definitions (phx PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions (phx PRIVATE WIN32_LEAN_AND_MEAN)
  target_compile_definitions (phx PRIVATE WINDOWS=1)

  target_compile_options (phx PRIVATE "/MP")         # Multithreaded Build
  target_compile_options (phx PRIVATE "/MD")         # Dynamic C Runtime
  target_compile_options (phx PRIVATE "/EHs-c-")     # No exception handling
  target_compile_options (phx PRIVATE "/fp:fast")    # No strict FP
  target_compile_options (phx PRIVATE "/GL")         # Whole Program Optimization
  target_compile_options (phx PRIVATE "/GS-")        # No Buffer Security Checks
  target_compile_options (phx PRIVATE "/GR-")        # No RTTI
  target_compile_options (phx PRIVATE "/arch:SSE2")  # Assume SSE2+

elseif (UNIX)

  target_link_libraries (phx
    GL
    GLEW
    dl
    SDL2
    freetype
    luajit-5.1
    lz4
    fmod fmodstudio
    BulletCollision BulletDynamics)

  target_compile_definitions (phx PRIVATE UNIX=1)

  target_compile_options (phx PRIVATE "-Wall")            # All error checking
  target_compile_options (phx PRIVATE "-fno-exceptions")  # No exception handling
  target_compile_options (phx PRIVATE "-ffast-math")      # No strict FP
  target_compile_options (phx PRIVATE "-fpic")            # PIC since this is shared

  target_compile_options (phx PRIVATE "-Wno-unused-variable")
  target_compile_options (phx PRIVATE "-Wno-unknown-pragmas")

  # Aggressive optimization, assuming SSE4+
  target_compile_options (phx PRIVATE "-O3")
  target_compile_options (phx PRIVATE "-msse")
  target_compile_options (phx PRIVATE "-msse2")
  target_compile_options (phx PRIVATE "-msse3")
  target_compile_options (phx PRIVATE "-msse4")

  # :(
  target_compile_options (phx PRIVATE "-std=c++11")

endif ()
