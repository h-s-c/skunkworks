## Public Domain
cmake_minimum_required (VERSION 3.0.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
cmake_policy(SET CMP0026 OLD)
include(AndroidNdkGdb)

project (skunkworks)
    if(MSVC)
        set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} /EHsc /W3 /wd4100 /wd4702 /Zc:strictStrings /Zc:rvalueCast /openmp")
    else()
        set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-missing-field-initializers -Wfatal-errors -std=c++1y -pedantic")
        set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-missing-field-initializers -Wfatal-errors -std=c11 -pedantic")
    endif()

    add_subdirectory(external)
    add_subdirectory(framework)
    add_subdirectory(plugins)
    
    find_package(ZMQ REQUIRED)
    list(APPEND SKUNKWORKS_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND SKUNKWORKS_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})

    find_package(EGL REQUIRED)
    list(APPEND SKUNKWORKS_INCLUDE_DIRS ${EGL_INCLUDE_DIR})
    
    list(APPEND SKUNKWORKS_LIBRARIES Framework)    
    list(APPEND SKUNKWORKS_SOURCE ${CMAKE_SOURCE_DIR}/main.cpp)
    list(APPEND SKUNKWORKS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})

    include_directories(${SKUNKWORKS_INCLUDE_DIRS} )
    if(ANDROID)
        add_library(Skunkworks SHARED ${SKUNKWORKS_SOURCE})
        android_ndk_gdb_enable()
        android_ndk_gdb_debuggable(Skunkworks)
        android_ndk_gdb_debuggable(PluginGraphics)
        android_ndk_gdb_debuggable(PluginGame)
        android_ndk_gdb_debuggable(PluginInput)
        android_ndk_gdb_debuggable(PluginPhysics)
    else()
        add_executable(Skunkworks ${SKUNKWORKS_SOURCE})
    endif()
    target_link_libraries(Skunkworks ${SKUNKWORKS_LIBRARIES})

