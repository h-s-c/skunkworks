##  Public Domain
##
##  EGL_FOUND
##  EGL_INCLUDE_DIR
##  EGL_LIBRARY
##

find_path(EGL_INCLUDE_DIR NAMES EGL/egl.h HINTS ${CMAKE_SOURCE_DIR}/external/khronos)
find_library(EGL_LIBRARY NAMES EGL PATH_SUFFIXES x86 x86-64 HINTS ${CMAKE_SOURCE_DIR}/external/khronos)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EGL DEFAULT_MSG EGL_LIBRARY EGL_INCLUDE_DIR)
mark_as_advanced(EGL_LIBRARY EGL_INCLUDE_DIR)

