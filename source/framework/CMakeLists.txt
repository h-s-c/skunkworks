## Public Domain

project (Framework)
    set( FRAMEWORK_SOURCE
        ${CMAKE_SOURCE_DIR}/framework/framework.cpp
    )
    list(APPEND FRAMEWORK_SOURCE ${FRAMEWORK_SOURCE})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)

    list(APPEND FRAMEWORK_LIBRARIES Zeug)

    find_package(EGL REQUIRED)
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${EGL_INCLUDE_DIR})

    if (UNIX)
        if (NOT ANDROID)
            if (NOT BLACKBERRY)   
                find_package(X11 REQUIRED)
                list(APPEND FRAMEWORK_INCLUDE_DIRS ${X11_INCLUDE_DIR})
            endif()
        endif()
        list(APPEND FRAMEWORK_LIBRARIES ${CMAKE_DL_LIBS})
        if(ANDROID)
            list(APPEND FRAMEWORK_LIBRARIES android)
        endif()
        if(BLACKBERRY)
            list(APPEND FRAMEWORK_LIBRARIES bps)
        endif()
    endif()
    
    find_package(ZMQ REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})

    include_directories(${FRAMEWORK_INCLUDE_DIRS} )
    link_directories(${FRAMEWORK_LIBRARY_DIRS} )
    add_library (Framework ${FRAMEWORK_SOURCE})
    target_link_libraries(Framework ${FRAMEWORK_LIBRARIES})