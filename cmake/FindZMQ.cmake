##  Public Domain
##
##  ZMQ_FOUND
##  ZMQ_INCLUDE_DIRS
##  ZMQ_LIBRARIES
##

find_path(ZMQ_INCLUDE_DIR NAMES zmq.hpp HINTS ${CMAKE_SOURCE_DIR}/external)
find_library(ZMQ_LIBRARY NAMES zmq)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZMQ DEFAULT_MSG ZMQ_LIBRARY ZMQ_INCLUDE_DIR)
mark_as_advanced(ZMQ_INCLUDE_DIR ZMQ_LIBRARY)
