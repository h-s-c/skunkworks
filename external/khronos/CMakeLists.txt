## Public Domain
##
##  dynamic Khronos API loader
##
project (DynEGL)
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
    add_library (DynEGL SHARED ${KHRONOS_SOURCE})
    
if (${CMAKE_C_COMPILER_ID} EQUAL "MSVC")
	set_source_files_properties(${KHRONOS_SOURCE} PROPERTIES LANGUAGE CXX)
	set_target_properties(DynEGL PROPERTIES LINKER_LANGUAGE CXX)
endif()

    install (TARGETS DynEGL
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS DynEGL
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  

project (DynGL)
    set(KHRONOS_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/khronos/GL/glstubs.c
        )
    list(APPEND KHRONOS_SOURCE ${KHRONOS_SOURCE})

    list(APPEND KHRONOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND KHRONOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    list(APPEND KHRONOS_LIBRARIES DynEGL)   
    
    if(NOT WIN32)
		find_package(GL REQUIRED)
		list(APPEND KHRONOS_LIBRARIES ${GL_LIBRARY})   
		list(APPEND KHRONOS_INCLUDE_DIRS ${GL_INCLUDE_DIR})
    endif()
    
    include_directories(${KHRONOS_INCLUDE_DIRS} )
    add_library (DynGL SHARED ${KHRONOS_SOURCE})
    target_link_libraries(DynGL ${KHRONOS_LIBRARIES})
    
if (${CMAKE_C_COMPILER_ID} EQUAL "MSVC")
	set_source_files_properties(${KHRONOS_SOURCE} PROPERTIES LANGUAGE CXX)
	set_target_properties(DynGL PROPERTIES LINKER_LANGUAGE CXX)
endif()
    
    install (TARGETS DynGL
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS DynGL
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
