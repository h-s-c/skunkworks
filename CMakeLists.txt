## Public Domain
cmake_minimum_required (VERSION 2.8.11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project (skunkworks)
    if(MSVC)
        set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} /EHsc /W3 /wd4100 /wd4702 /Zc:strictStrings /Zc:rvalueCast")
    else()
        set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wfatal-errors -std=c++11 -pedantic")
        set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wfatal-errors -std=c11 -pedantic")
    endif()

    add_subdirectory(base)
    add_subdirectory(external)
    add_subdirectory(framework)
    add_subdirectory(plugins)
    
    find_package(ZMQ REQUIRED)
    list(APPEND SKUNKWORKS_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND SKUNKWORKS_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})
    
    list(APPEND SKUNKWORKS_LIBRARIES Framework)    
    list(APPEND SKUNKWORKS_SOURCE ${CMAKE_SOURCE_DIR}/main.cpp)
    list(APPEND SKUNKWORKS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})

    include_directories(${SKUNKWORKS_INCLUDE_DIRS} )
    add_executable(Skunkworks ${SKUNKWORKS_SOURCE})
    target_link_libraries(Skunkworks ${SKUNKWORKS_LIBRARIES})

