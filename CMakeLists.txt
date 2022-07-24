cmake_minimum_required (VERSION 3.0.2)
project (LibPHX)
set (CMAKE_BUILD_TYPE Release)
include (script/build/Shared.cmake)

# ------------------------------------------------------------------------------

file (GLOB HEADERS "include/*.h")
file (GLOB SOURCES "src/*.cpp")

add_library (phx SHARED ${SOURCES} ${HEADERS})
phx_configure_output_dir (phx)
phx_configure_target_properties (phx)

target_compile_definitions (phx PRIVATE LIBPHX_BUILDING=1)
target_include_directories (phx PUBLIC "include")
target_include_directories (phx PUBLIC "ext/include")
target_include_directories (phx PUBLIC "ext/include/bullet")
target_link_directories (phx PUBLIC "ext/lib/${PLATARCH}")

set_target_properties (phx PROPERTIES
  PREFIX "lib"
  OUTPUT_NAME_DEBUG "phx${ARCH}d"
  OUTPUT_NAME_RELEASE "phx${ARCH}r"
  OUTPUT_NAME_RELWITHDEBINFO "phx${ARCH}"
  OUTPUT_NAME_MINSIZEREL "phx${ARCH}rm")

# ------------------------------------------------------------------------------

if (WINDOWS)

  # register an external shared library
  function (add_extlib name lib dll)
    add_library (${name} SHARED IMPORTED GLOBAL)
    set_property (TARGET ${name} PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/ext/lib/${PLATARCH}/${lib}")
    set_property (TARGET ${name} PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/ext/lib/${PLATARCH}/${dll}")
  endfunction ()

  # link target to an external library; also add a post-build step to
  # copy the library DLL into the bin folder
  function (target_link_extlib target lib)
    target_link_libraries (${target} ${lib})
    add_custom_command (TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:${lib}> $<TARGET_FILE_DIR:${target}>
    )
  endfunction ()

  add_extlib (glew glew32.lib glew32.dll)
  add_extlib (lz4 liblz4.lib liblz4.dll)
  add_extlib (lua51 lua51.lib lua51.dll)
  add_extlib (sdl2 SDL2.lib SDL2.dll)

  if (ARCH EQUAL "32")
    add_extlib (fmod fmodL_vc.lib fmodL.dll)
    add_extlib (fmodstudio fmodstudioL_vc.lib fmodstudioL.dll)
  else ()
    add_extlib (fmod fmodL64_vc.lib fmodL64.dll)
    add_extlib (fmodstudio fmodstudioL64_vc.lib fmodstudioL64.dll)
  endif ()

elseif (LINUX)

  # TODO

endif ()

# ------------------------------------------------------------------------------

if (WINDOWS)

  target_link_libraries (phx
    opengl32.lib
    user32.lib winmm.lib Ws2_32.lib
    freetype.lib
    BulletCollision.lib BulletDynamics.lib LinearMath.lib)

  target_link_extlib (phx glew)
  target_link_extlib (phx sdl2)
  target_link_extlib (phx lz4)
  target_link_extlib (phx lua51)
  target_link_extlib (phx fmod)
  target_link_extlib (phx fmodstudio)

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

  # Add ext/lib/{$PLATARCH} to runtime library search path
  target_link_options (phx PRIVATE "-Wl,-rpath,../ext/lib/${PLATARCH}")

endif ()
