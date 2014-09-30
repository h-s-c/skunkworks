## Public Domain

add_subdirectory(OIS)
add_subdirectory(zeug)

project (Json++)
    set(JSONXX_SOURCE  
            ${CMAKE_CURRENT_LIST_DIR}/jsonxx.cpp
        )

    list(APPEND JSONXX_SOURCE ${JSONXX_SOURCE})

    list(APPEND JSONXX_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR})
    
    include_directories(${JSONXX_INCLUDE_DIRS} )
    add_library (Json++ STATIC ${JSONXX_SOURCE})
    set_target_properties(Json++ PROPERTIES POSITION_INDEPENDENT_CODE TRUE)