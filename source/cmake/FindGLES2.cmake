##  Public Domain
##
##  GLES2_FOUND
##  GLES2_INCLUDE_DIR
##  GLES2_LIBRARY
##

find_path(GLES2_INCLUDE_DIR NAMES GLES2/gl2.h)
find_library(GLES2_LIBRARY NAMES GLESv2)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLES2 DEFAULT_MSG GLES2_LIBRARY GLES2_INCLUDE_DIR)
mark_as_advanced(GLES2_LIBRARY GLES2_INCLUDE_DIR)

