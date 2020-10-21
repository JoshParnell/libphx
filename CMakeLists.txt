cmake_minimum_required (VERSION 3.0.2)
project (LibPHX)
set (CMAKE_BUILD_TYPE Release)

if (WIN32)
  set (PLATFORM "win")
  set (WINDOWS TRUE)
elseif (UNIX AND NOT APPLE)
  set (PLATFORM "linux")
  set (LINUX TRUE)
else ()
  message (FATAL_ERROR "Unsupported Platform")
endif ()

if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
  set (ARCH "32")
elseif ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
  set (ARCH "64")
else ()
  message (FATAL_ERROR "Unsupported CPU Architecture")
endif ()

set (PLATARCH "${PLATFORM}${ARCH}")

# ------------------------------------------------------------------------------

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATARCH}")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATARCH}")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATARCH}")

foreach (config ${CMAKE_CONFIGURATION_TYPES})
  string (TOUPPER ${config} config)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_SOURCE_DIR}/bin/${PLATARCH}")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config} "${CMAKE_SOURCE_DIR}/bin/${PLATARCH}")
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config} "${CMAKE_SOURCE_DIR}/bin/${PLATARCH}")
endforeach (config)

file (GLOB HEADERS "include/*.h")
file (GLOB SOURCES "src/*.cpp")

add_library (phx SHARED ${SOURCES} ${HEADERS})

target_compile_definitions (phx PRIVATE LIBPHX_BUILDING=1)
target_include_directories (phx PUBLIC "include")
target_include_directories (phx PUBLIC "ext/include")
target_include_directories (phx PUBLIC "ext/include/bullet")
target_link_directories (phx PUBLIC "ext/lib/${PLATARCH}")

set_target_properties (phx PROPERTIES OUTPUT_NAME_DEBUG "phx${ARCH}d")
set_target_properties (phx PROPERTIES OUTPUT_NAME_RELEASE "phx${ARCH}r")
set_target_properties (phx PROPERTIES OUTPUT_NAME_RELWITHDEBINFO "phx${ARCH}")
set_target_properties (phx PROPERTIES OUTPUT_NAME_MINSIZEREL "phx${ARCH}rm")

# ------------------------------------------------------------------------------

if (WINDOWS)
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

elseif (LINUX)

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

  # Add ext/lib/{$PLATARCH} to runtime library search path
  target_link_options (phx PRIVATE "-Wl,-rpath,../ext/lib/${PLATARCH}")

endif ()
