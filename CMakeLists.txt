# Hack to suppress message output when the status is not specified or it is a
# generic status message.
function(message)
  list(GET ARGV 0 MessageType)
  if(MessageType STREQUAL FATAL_ERROR OR
     MessageType STREQUAL SEND_ERROR OR
     MessageType STREQUAL WARNING OR
     MessageType STREQUAL AUTHOR_WARNING)
    list(REMOVE_AT ARGV 0)
    _message(${MessageType} "${ARGV}")
  endif()
endfunction()

# Keep track of the current directory.
set (ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# CMakeLists files in this project can refer to the root source directory of
# the project as '${GUNGI_SOURCE_DIR}' and to the root binary directory of
# the project as '${GUNGI_BINARY_DIR}'.
cmake_minimum_required (VERSION 3.4.1)
project (GUNGI)

# Recurse into the subdirectories.
add_subdirectory (submodules)
add_subdirectory (gungi)
add_subdirectory (demo)
