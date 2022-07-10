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
