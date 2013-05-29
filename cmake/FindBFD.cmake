##  Public Domain
##
##  BFD_FOUND
##  BFD_INCLUDE_DIR
##  BFD_LIBRARY
##

find_path(BFD_INCLUDE_DIR NAMES bfd.h)
find_library(BFD_LIBRARY NAMES bfd PATH_SUFFIXES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BFD DEFAULT_MSG BFD_LIBRARY BFD_INCLUDE_DIR)
mark_as_advanced(BFD_LIBRARY BFD_INCLUDE_DIR)

