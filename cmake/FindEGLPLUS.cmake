##  Public Domain
##
##  EGLPLUS_FOUND
##  EGLPLUS_INCLUDE_DIR
##

find_path(EGLPLUS_INCLUDE_DIR NAMES eglplus/egl.hpp HINTS ${CMAKE_SOURCE_DIR}/external/chochlik)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EGLPLUS DEFAULT_MSG EGLPLUS_INCLUDE_DIR)
mark_as_advanced(EGLPLUS_INCLUDE_DIR)

