##  Public Domain
##
##  MSGPACK_FOUND
##  MSGPACK_INCLUDE_DIR
##  MSGPACK_LIBRARY
##

find_path(MSGPACK_INCLUDE_DIR NAMES msgpack.hpp)
find_library(MSGPACK_LIBRARY NAMES msgpack libmsgpack)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MSGPACK DEFAULT_MSG MSGPACK_LIBRARY MSGPACK_INCLUDE_DIR)
mark_as_advanced(MSGPACK_LIBRARY MSGPACK_INCLUDE_DIR)

