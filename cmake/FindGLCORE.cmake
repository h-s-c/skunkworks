##  Public Domain
##
##  GLCORE_FOUND
##  GLCORE_INCLUDE_DIR
##  GLCORE_LIBRARY
##

find_path(GLCORE_INCLUDE_DIR NAMES GL/glcorearb.h HINTS ${CMAKE_SOURCE_DIR}/external/khronos)
find_library(GLCORE_LIBRARY NAMES GLcore PATH_SUFFIXES x86 x86-64 HINTS ${CMAKE_SOURCE_DIR}/external/khronos)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLCORE DEFAULT_MSG GLCORE_LIBRARY GLCORE_INCLUDE_DIR)
mark_as_advanced(GLCORE_LIBRARY GLCORE_INCLUDE_DIR)

