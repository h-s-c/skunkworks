## Public Domain

project (Framework)
    set( FRAMEWORK_SOURCE
        ${CMAKE_SOURCE_DIR}/framework/framework.cpp
    )
    list(APPEND FRAMEWORK_SOURCE ${FRAMEWORK_SOURCE})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)
    
    list(APPEND FRAMEWORK_LIBRARIES Base)
    
    find_package(ZMQ REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})
    
    include_directories(${FRAMEWORK_INCLUDE_DIRS} )
    link_directories(${FRAMEWORK_LIBRARY_DIRS} )
    add_library (Framework ${FRAMEWORK_SOURCE})
    target_link_libraries(Framework ${FRAMEWORK_LIBRARIES})
