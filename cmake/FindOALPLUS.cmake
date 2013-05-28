##  Public Domain
##
##  OALPLUS_FOUND
##  OALPLUS_INCLUDE_DIR
##

find_path(OALPLUS_INCLUDE_DIR NAMES oalplus/al.hpp HINTS ${CMAKE_SOURCE_DIR}/external)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OALPLUS DEFAULT_MSG OALPLUS_INCLUDE_DIR)
mark_as_advanced(OALPLUS_INCLUDE_DIR)

