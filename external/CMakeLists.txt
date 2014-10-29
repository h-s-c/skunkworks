## Public Domain

add_subdirectory(OIS)
add_subdirectory(zeug)

project (json11)
    set(JSON11_SOURCE  
            ${CMAKE_CURRENT_LIST_DIR}/json11/json11.cpp
        )

    list(APPEND JSON11_SOURCE ${JSON11_SOURCE})

    list(APPEND JSON11_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/json11)
    
    include_directories(${JSON11_INCLUDE_DIRS} )
    add_library (json11 STATIC ${JSON11_SOURCE})
    set_target_properties(json11 PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

project (imgui)
    set(IMGUI_SOURCE  
            ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui.cpp
        )

    list(APPEND IMGUI_SOURCE ${IMGUI_SOURCE})

    list(APPEND IMGUI_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/imgui)
    
    include_directories(${IMGUI_INCLUDE_DIRS} )
    add_library (IMGUI STATIC ${IMGUI_SOURCE})
    set_target_properties(IMGUI PROPERTIES POSITION_INDEPENDENT_CODE TRUE)