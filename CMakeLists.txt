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
  
  target_compile_options (phx PRIVATE "/MP")			# Multithreaded Build
  target_compile_options (phx PRIVATE "/MD")			# Dynamic C Runtime
  target_compile_options (phx PRIVATE "/EHs-c-")		# No exception handling
  target_compile_options (phx PRIVATE "/fp:fast")		# Strict FP not needed
  target_compile_options (phx PRIVATE "/GL")			# Whole Program Optimization
  target_compile_options (phx PRIVATE "/GS-")			# No Buffer Security Checks
  target_compile_options (phx PRIVATE "/GR-")			# No RTTI
  target_compile_options (phx PRIVATE "/arch:SSE2")		# Assume SSE2+
endif ()
