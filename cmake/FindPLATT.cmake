##  Public Domain
##
##  PLATT_FOUND
##  PLATT_INCLUDE_DIR
##

find_path(PLATT_INCLUDE_DIR NAMES platt/platform.hpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PLATT DEFAULT_MSG PLATT_INCLUDE_DIR)
mark_as_advanced(PLATT_INCLUDE_DIR)

