##  Public Domain
##
##  GL_FOUND
##  GL_INCLUDE_DIR
##  GL_LIBRARY
##

find_path(GL_INCLUDE_DIR NAMES GL/gl.h)
find_library(GL_LIBRARY NAMES GL PATH_SUFFIXES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GL DEFAULT_MSG GL_LIBRARY GL_INCLUDE_DIR)
mark_as_advanced(GL_LIBRARY GL_INCLUDE_DIR)

