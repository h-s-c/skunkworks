##  Public Domain
##
##  ZMQ_FOUND
##  ZMQ_INCLUDE_DIRS
##  ZMQ_LIBRARIES
##

find_path(ZMQ_INCLUDE_DIR NAMES zmq.h PATHS "$ENV{PROGRAMFILES}/ZeroMQ 3.2.3/include" "$ENV{PROGRAMW6432}/ZeroMQ 3.2.3/include")
find_library(ZMQ_LIBRARY NAMES zmq libzmq-v110-mt-3_2_3.lib PATHS "$ENV{PROGRAMFILES}/ZeroMQ 3.2.3/lib" "$ENV{PROGRAMW6432}/ZeroMQ 3.2.3/lib")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZMQ DEFAULT_MSG ZMQ_LIBRARY ZMQ_INCLUDE_DIR)
mark_as_advanced(ZMQ_INCLUDE_DIR ZMQ_LIBRARY)
