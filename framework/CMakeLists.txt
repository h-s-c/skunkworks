## Public Domain

project (Framework)
    set( FRAMEWORK_SOURCE
        ${CMAKE_SOURCE_DIR}/framework/framework.cpp
    )
    list(APPEND FRAMEWORK_SOURCE ${FRAMEWORK_SOURCE})
    
    list(APPEND FRAMEWORK_LIBRARIES Base)
    
    find_package(SDL2 REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${SDL2_LIBRARY}) 
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    
    find_package(ZMQ REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${ZMQ_LIBRARIES})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${ZMQ_INCLUDE_DIRS})
    
    find_package(CZMQ REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${CZMQ_LIBRARIES})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${CZMQ_INCLUDE_DIRS})
    
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)
    
    include_directories(${FRAMEWORK_INCLUDE_DIRS} )
    link_directories(${FRAMEWORK_LIBRARY_DIRS} )
    add_library (Framework ${FRAMEWORK_SOURCE})
    target_link_libraries(Framework ${FRAMEWORK_LIBRARIES})
