## Public Domain

add_subdirectory(OIS)
add_subdirectory(zeug)

project (jsonxx)
    set(JSONXX_SOURCE  
            ${CMAKE_CURRENT_LIST_DIR}/jsonxx/jsonxx.cc
        )

    list(APPEND JSONXX_SOURCE ${JSONXX_SOURCE})

    list(APPEND JSONXX_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/jsonxx)
    
    include_directories(${JSONXX_INCLUDE_DIRS} )
    add_library (jsonxx STATIC ${JSONXX_SOURCE})
    set_target_properties(jsonxx PROPERTIES POSITION_INDEPENDENT_CODE TRUE)