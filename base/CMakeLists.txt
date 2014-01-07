## Public Domain
project (Base)
    set(BASE_SOURCE    
        ${CMAKE_SOURCE_DIR}/base/parser/json.cpp
        )
    list(APPEND BASE_SOURCE ${BASE_SOURCE})
    
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})

    include_directories(${BASE_INCLUDE_DIRS} )
    add_library (Base ${BASE_SOURCE})
    target_link_libraries(Base ${BASE_LIBRARIES})
    set_target_properties(Base PROPERTIES POSITION_INDEPENDENT_CODE TRUE)