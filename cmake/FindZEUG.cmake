##  Public Domain
##
##  ZEUG_FOUND
##  ZEUG_INCLUDE_DIR
##

find_path(ZEUG_INCLUDE_DIR NAMES zeug/platform.hpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZEUG DEFAULT_MSG ZEUG_INCLUDE_DIR)
mark_as_advanced(ZEUG_INCLUDE_DIR)

