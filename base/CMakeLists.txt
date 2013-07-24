## Public Domain
project (Base)
    set(BASE_SOURCE    
        ${CMAKE_SOURCE_DIR}/base/system/info.cpp
        ${CMAKE_SOURCE_DIR}/base/system/library.cpp
        ${CMAKE_SOURCE_DIR}/base/system/window.cpp
        ${CMAKE_SOURCE_DIR}/base/parser/json.cpp
        )
    list(APPEND BASE_SOURCE ${BASE_SOURCE})
    
    if (NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
        if(UNIX)
            set(UNIX_SOURCE    
                ${CMAKE_SOURCE_DIR}/base/system/stacktrace.cpp
                )
            list(APPEND BASE_SOURCE ${UNIX_SOURCE})
        endif()
    endif()
    
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)

    if (UNIX)
        find_package(X11 REQUIRED)
        list(APPEND BASE_LIBRARIES ${X11_LIBRARY})   
        list(APPEND BASE_INCLUDE_DIRS ${X11_INCLUDE_DIR})
        
        find_package(BFD)
        if (BFD_FOUND)
            list(APPEND BASE_LIBRARIES ${BFD_LIBRARY})
            list(APPEND BASE_INCLUDE_DIRS ${BFD_INCLUDE_DIR})
        endif ()
        list(APPEND BASE_LIBRARIES ${CMAKE_DL_LIBS})
    endif()
    
    if(USE_SDL2)
        add_definitions(-DUSE_SDL2)        
        find_package(SDL2 REQUIRED)
        list(APPEND BASE_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
        list(APPEND BASE_LIBRARIES ${SDL2_LIBRARY})
    endif()
    
    include_directories(${BASE_INCLUDE_DIRS} )
    add_library (Base ${BASE_SOURCE})
    target_link_libraries(Base ${BASE_LIBRARIES})
    set_target_properties(Base PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
