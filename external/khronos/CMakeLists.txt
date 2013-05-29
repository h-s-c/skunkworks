## Public Domain
##
## Khronos loader
##
project (KhronosWrapEGL)
    set(KHRONOS_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/khronos/GL/gl.c
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES/gles.c
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES2/gles2.c
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES3/gles3.c
        )
    list(APPEND KHRONOS_SOURCE ${KHRONOS_SOURCE})
    

    if(WIN32)
        list(APPEND KHRONOS_SOURCE EGL/egl_wgl.c)
    endif()
         
    if (UNIX)
        if (APPLE)
            list(APPEND KHRONOS_SOURCE EGL/egl_agl.m)
        else()
            list(APPEND KHRONOS_SOURCE EGL/egl_glx.c)
        endif()
    endif()   
    
    list(APPEND KHRONOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND KHRONOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    include_directories(${KHRONOS_INCLUDE_DIRS} )
    add_library (KhronosWrapEGL ${KHRONOS_SOURCE})
    set_target_properties(KhronosWrapEGL PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

project (KhronosLoadGL)
    set(KHRONOS_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/khronos/GL/glstubs.c
        )
    list(APPEND KHRONOS_SOURCE ${KHRONOS_SOURCE})

    list(APPEND KHRONOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND KHRONOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    include_directories(${KHRONOS_INCLUDE_DIRS} )
    add_library (KhronosLoadGL ${KHRONOS_SOURCE})
    set_target_properties(KhronosLoadGL PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
