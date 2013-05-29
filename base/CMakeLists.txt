## Public Domain
project (Base)
    set(BASE_SOURCE    
        ${CMAKE_SOURCE_DIR}/base/system/info.cpp
        ${CMAKE_SOURCE_DIR}/base/system/library.cpp
        ${CMAKE_SOURCE_DIR}/base/system/stacktrace.cpp
        ${CMAKE_SOURCE_DIR}/base/system/window.cpp
        )
    list(APPEND BASE_SOURCE ${BASE_SOURCE})
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    if(NOT ANDROID) 
        find_package(SDL2 REQUIRED)
        list(APPEND BASE_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
        list(APPEND BASE_LIBRARIES ${SDL2_LIBRARY})
    endif()
    
    find_package(BFD)
    if (BFD_FOUND)
        list(APPEND BASE_LIBRARIES ${BFD_LIBRARY})
        list(APPEND BASE_INCLUDE_DIRS ${BFD_INCLUDE_DIR})
    endif ()
    
    if(UNIX) 
        list(APPEND BASE_LIBRARIES ${CMAKE_DL_LIBS})
    endif() 
    
    include_directories(${BASE_INCLUDE_DIRS} )
    add_library (Base ${BASE_SOURCE})
    target_link_libraries(Base ${BASE_LIBRARIES})
    set_target_properties(Base PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
