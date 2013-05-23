## Public Domain
project (Base)
    set(BASE_SOURCE    
        ${CMAKE_SOURCE_DIR}/base/system/info.cpp
        ${CMAKE_SOURCE_DIR}/base/system/library.cpp
        )
    list(APPEND BASE_SOURCE ${BASE_SOURCE})
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    if (UNIX) 
        ##dlfcn
        list(APPEND BASE_LIBRARIES ${CMAKE_DL_LIBS})
    else (MSVC AND USE_DLFCN_WIN32) 
        ##dlfcn-win32
        add_definitions(-DUSE_DLFCN_WIN32)
        list(APPEND BASE_SOURCE ${CMAKE_SOURCE_DIR}/external/dlfcn-win32/dlfcn.c)
        list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/dlfcn-win32)
    endif () 
    
    include_directories(${BASE_INCLUDE_DIRS} )
    add_library (Base ${BASE_SOURCE})
    target_link_libraries(Base ${BASE_LIBRARIES})
    set_target_properties(Base PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
