## Public Domain

project (PluginGame)
    set( PLUGIN_SOURCE
        ${CMAKE_SOURCE_DIR}/plugins/game/plugin.cpp
        ${CMAKE_SOURCE_DIR}/plugins/game/entitymanager.cpp
    )
    list(APPEND PLUGIN_SOURCE ${PLUGIN_SOURCE})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)

    list(APPEND PLUGIN_LIBRARIES Zeug)

    if (UNIX)
        list(APPEND PLUGIN_LIBRARIES ${CMAKE_DL_LIBS})
    endif()
    
    find_package(ZMQ REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})
    
    find_package(MSGPACK REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${MSGPACK_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIR})
        
    include_directories(${PLUGIN_INCLUDE_DIRS} )
    add_library(PluginGame SHARED ${PLUGIN_SOURCE})
    target_link_libraries(PluginGame ${PLUGIN_LIBRARIES})