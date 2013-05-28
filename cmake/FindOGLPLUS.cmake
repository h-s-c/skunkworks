##  Public Domain
##
##  OGLPLUS_FOUND
##  OGLPLUS_INCLUDE_DIR
##

find_path(OGLPLUS_INCLUDE_DIR NAMES oglplus/gl.hpp HINTS ${CMAKE_SOURCE_DIR}/external)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OGLPLUS DEFAULT_MSG OGLPLUS_INCLUDE_DIR)
mark_as_advanced(OGLPLUS_INCLUDE_DIR)

