## Public Domain
project (EGL)
    set(EGL_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/khronos/GL/gl.c
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES/gles.c
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES2/gles2.c
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES3/gles3.c
        )
    list(APPEND EGL_SOURCE ${EGL_SOURCE})
    

    if(APPLE)
        list(APPEND EGL_SOURCE WGL/egl_wgl.c)
    endif()
         
    if (WIN32) 
        list(APPEND EGL_SOURCE GLX/egl_glx.c)
    endif()   
    
    list(APPEND EGL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND EGL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    include_directories(${EGL_INCLUDE_DIRS} )
    add_library (EGL SHARED ${EGL_SOURCE})

    
project (GLcore)
    set(GLcore_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/khronos/GL/glstubs.c
        )
    list(APPEND GLcore_SOURCE ${GLcore_SOURCE})
    
    list(APPEND GLcore_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND GLcore_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    include_directories(${GLcore_INCLUDE_DIRS} )
    add_library (GLcore SHARED ${GLcore_SOURCE})
    
project (GLES)
    set(GLES_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES/glesstubs.c
        )
    list(APPEND GLES_SOURCE ${GLES_SOURCE})
    
    list(APPEND GLES_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND GLES_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    include_directories(${GLES_INCLUDE_DIRS} )
    add_library (GLES SHARED ${GLES_SOURCE})
    
project (GLESv2)
    set(GLESv2_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES2/gles2stubs.c
        )
    list(APPEND GLESv2_SOURCE ${GLESv2_SOURCE})
    
    list(APPEND GLESv2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND GLESv2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    include_directories(${GLESv2_INCLUDE_DIRS} )
    add_library (GLESv2 SHARED ${GLESv2_SOURCE})
    
project (GLESv3)
    set(GLESv3_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/khronos/GLES3/gles3stubs.c
        )
    list(APPEND GLESv3_SOURCE ${GLESv3_SOURCE})
    
    list(APPEND GLESv3_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND GLESv3_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    include_directories(${GLESv3_INCLUDE_DIRS} )
    add_library (GLESv3 SHARED ${GLESv3_SOURCE})
