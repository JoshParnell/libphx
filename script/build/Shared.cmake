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

function (phx_configure_output_dir target)
  set_target_properties (${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

  foreach (config ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${config} config)
    set_target_properties (${target} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_SOURCE_DIR}/bin"
      LIBRARY_OUTPUT_DIRECTORY_${config} "${CMAKE_SOURCE_DIR}/bin"
      ARCHIVE_OUTPUT_DIRECTORY_${config} "${CMAKE_SOURCE_DIR}/bin")
  endforeach (config)
endfunction ()

function (phx_configure_target_properties target)
  if (WINDOWS)
    target_compile_definitions (${target} PRIVATE _CRT_SECURE_NO_DEPRECATE)
    target_compile_definitions (${target} PRIVATE WIN32_LEAN_AND_MEAN)
    target_compile_definitions (${target} PRIVATE WINDOWS=1)

    target_compile_options (${target} PRIVATE "/MP")         # Multithreaded Build
    target_compile_options (${target} PRIVATE "/MD")         # Dynamic C Runtime
    target_compile_options (${target} PRIVATE "/EHs-c-")     # No exception handling
    target_compile_options (${target} PRIVATE "/fp:fast")    # No strict FP
    target_compile_options (${target} PRIVATE "/GL")         # Whole Program Optimization
    target_compile_options (${target} PRIVATE "/GS-")        # No Buffer Security Checks
    target_compile_options (${target} PRIVATE "/GR-")        # No RTTI
    target_compile_options (${target} PRIVATE "/arch:SSE2")  # Assume SSE2+
  elseif (LINUX)
    target_compile_definitions (${target} PRIVATE UNIX=1)

    target_compile_options (${target} PRIVATE "-Wall")            # All error checking
    target_compile_options (${target} PRIVATE "-fno-exceptions")  # No exception handling
    target_compile_options (${target} PRIVATE "-ffast-math")      # No strict FP
    target_compile_options (${target} PRIVATE "-fpic")            # PIC since this is shared

    target_compile_options (${target} PRIVATE "-Wno-unused-variable")
    target_compile_options (${target} PRIVATE "-Wno-unknown-pragmas")

    # Aggressive optimization, assuming SSE4+
    target_compile_options (${target} PRIVATE "-O3")
    target_compile_options (${target} PRIVATE "-msse")
    target_compile_options (${target} PRIVATE "-msse2")
    target_compile_options (${target} PRIVATE "-msse3")
    target_compile_options (${target} PRIVATE "-msse4")

    # :(
    target_compile_options (${target} PRIVATE "-std=c++11")

  endif ()
endfunction ()
