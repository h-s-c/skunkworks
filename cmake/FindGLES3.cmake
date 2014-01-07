##  Public Domain
##
##  GLES3_FOUND
##  GLES3_INCLUDE_DIR
##  GLES3_LIBRARY
##

find_path(GLES3_INCLUDE_DIR NAMES GLES3/gl3.h)
find_library(GLES3_LIBRARY NAMES GLESv2 PATH_SUFFIXES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLES3 DEFAULT_MSG GLES3_LIBRARY GLES3_INCLUDE_DIR)
mark_as_advanced(GLES3_LIBRARY GLES3_INCLUDE_DIR)

