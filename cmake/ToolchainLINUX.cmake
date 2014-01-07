set(CMAKE_SYSTEM_NAME Linux)

if(NOT DEFINED ENV{TOOLCHAIN})
  message("TOOLCHAIN is not set; You must tell CMake where to find TOOLCHAIN")
  return()
endif()

set(CMAKE_FIND_ROOT_PATH $ENV{TOOLCHAIN})