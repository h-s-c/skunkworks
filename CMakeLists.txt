## Public Domain
cmake_minimum_required (VERSION 2.8.10)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project (skunkworks)
    ## Compiler-specific C++11 activation/checks.
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.7))
            message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
        endif ()
        set(CMAKE_CXX_FLAGS                "-D_GLIBCXX_USE_NANOSLEEP -Wall -Wextra -Wno-unused -std=c++11 -pedantic")
        set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
        set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
        add_definitions(-D_GLIBCXX_USE_CLOCK_REALTIME)
        add_definitions(-D_GLIBCXX_USE_SCHED_YIELD)
    elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 3.0))
            message(FATAL_ERROR "${PROJECT_NAME} requires clang 3.0 or greater.")
        endif ()
        ## -Wno-ignored-qualifiers and -Wno-overloaded-virtual silence warnings in OGRE headers
        set(CMAKE_CXX_FLAGS                "-D_GLIBCXX_USE_NANOSLEEP -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8 -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-overloaded-virtual -std=c++11 -pedantic -fPIC")
        set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
        set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
        add_definitions(-D_GLIBCXX_USE_CLOCK_REALTIME)
        add_definitions(-D_GLIBCXX_USE_SCHED_YIELD)
    elseif (${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
        ## /wd4251 silences warnings in OGRE headers
        set(CMAKE_CXX_FLAGS                "/EHsc /wd4251 /D_CRT_SECURE_NO_WARNINGS")
        ##set(CMAKE_CXX_FLAGS_DEBUG          "")
        ##set(CMAKE_CXX_FLAGS_MINSIZEREL     "")
        ##set(CMAKE_CXX_FLAGS_RELEASE        "")
        ##set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
        if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 17.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 17.0))
            message(FATAL_ERROR "${PROJECT_NAME} requires vs2012 or greater.")
        endif ()
    elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
        set(CMAKE_CXX_FLAGS                "-Wall -Wextra -Wno-unused -std=c++11 -pedantic -fPIC ")
        set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
        set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
        if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 13.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 13.0))
            message(FATAL_ERROR "${PROJECT_NAME} requires icc 13 or greater.")
        endif ()
    else ()
        message(FATAL_ERROR "Your C++ compiler does not have the required C++11 support.")
    endif ()
    
    ## CMake defaults to leaving CMAKE_BUILD_TYPE empty. 
    ## This screws updifferentiation between debug and release builds.   
    if (CMAKE_BUILD_TYPE STREQUAL "")
      set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
    endif ()
    
    add_subdirectory(base)
    add_subdirectory(framework)
    add_subdirectory(plugins/graphics)
    add_subdirectory(plugins/input)
    
    find_package(ZMQ REQUIRED)
    list(APPEND SKUNKWORKS_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND SKUNKWORKS_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})
    
    list(APPEND SKUNKWORKS_LIBRARIES Framework)    
    list(APPEND SKUNKWORKS_SOURCE ${CMAKE_SOURCE_DIR}/main.cpp)
    list(APPEND SKUNKWORKS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})

    include_directories(${SKUNKWORKS_INCLUDE_DIRS} )
    add_executable(Skunkworks ${SKUNKWORKS_SOURCE})
    target_link_libraries(Skunkworks ${SKUNKWORKS_LIBRARIES})
    
    install (TARGETS Skunkworks 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release
        CONFIGURATIONS Release RelWithDebInfo)
    install (TARGETS Skunkworks
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug
        CONFIGURATIONS Debug)

