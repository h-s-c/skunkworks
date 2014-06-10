## Public Domain

project (PluginInput)
    set( PLUGIN_SOURCE
        ${CMAKE_SOURCE_DIR}/plugins/input/plugin.cpp
    )
    list(APPEND PLUGIN_SOURCE ${PLUGIN_SOURCE})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)
    
    list(APPEND PLUGIN_LIBRARIES OIS)
    list(APPEND PLUGIN_LIBRARIES Zeug)

    if (UNIX)
        if(NOT ANDROID)
            find_package(X11 REQUIRED)
            list(APPEND PLUGIN_LIBRARIES ${X11_LIBRARY})   
            list(APPEND PLUGIN_INCLUDE_DIRS ${X11_INCLUDE_DIR})
        endif()
        list(APPEND PLUGIN_LIBRARIES ${CMAKE_DL_LIBS})
    endif()
    
    find_package(ZMQ REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})
        
    include_directories(${PLUGIN_INCLUDE_DIRS} )
    add_library(PluginInput SHARED ${PLUGIN_SOURCE})
    target_link_libraries(PluginInput ${PLUGIN_LIBRARIES})
    set_target_properties(PluginInput PROPERTIES PREFIX "")
