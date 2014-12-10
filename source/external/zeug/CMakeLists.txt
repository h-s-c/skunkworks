project (Zeug)
    set(ZEUG_SOURCE  
            ${CMAKE_CURRENT_LIST_DIR}/memory_map.cpp
            ${CMAKE_CURRENT_LIST_DIR}/platform.cpp
            ${CMAKE_CURRENT_LIST_DIR}/shared_lib.cpp
            ${CMAKE_CURRENT_LIST_DIR}/window.cpp
            ${CMAKE_CURRENT_LIST_DIR}/zipreader.cpp
            ${CMAKE_CURRENT_LIST_DIR}/opengl/buffer.cpp
            ${CMAKE_CURRENT_LIST_DIR}/opengl/common.cpp
            ${CMAKE_CURRENT_LIST_DIR}/opengl/program.cpp
            ${CMAKE_CURRENT_LIST_DIR}/opengl/shader.cpp
            ${CMAKE_CURRENT_LIST_DIR}/opengl/texture.cpp
            ${CMAKE_CURRENT_LIST_DIR}/thirdparty/miniz.c
        )

    list(APPEND ZEUG_SOURCE ${ZEUG_SOURCE})

    if(UNIX)
        if(ANDROID)
            list(APPEND ZEUG_SOURCE ${CMAKE_CURRENT_LIST_DIR}/thirdparty/cpu-features.c)
        endif()
    endif()

    find_package(EGL REQUIRED)
    list(APPEND ZEUG_INCLUDE_DIRS ${EGL_INCLUDE_DIR})

    find_package(IPP REQUIRED)
    list(APPEND ZEUG_LIBRARIES ${IPP_LIBRARIES})   
    list(APPEND ZEUG_INCLUDE_DIRS ${IPP_INCLUDE_DIRS})

    list(APPEND ZEUG_SOURCE ${ZEUG_SOURCE})

    list(APPEND ZEUG_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR})
    list(APPEND ZEUG_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/..)
    
    include_directories(${ZEUG_INCLUDE_DIRS} )
    add_library (Zeug STATIC ${ZEUG_SOURCE})
    target_link_libraries(Zeug ${ZEUG_LIBRARIES})
    set_target_properties(Zeug PROPERTIES POSITION_INDEPENDENT_CODE TRUE)