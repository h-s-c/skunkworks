## Public Domain

project (PluginGraphics)
    set( PLUGIN_SOURCE
        ${CMAKE_SOURCE_DIR}/plugins/graphics/plugin.cpp
        ${CMAKE_SOURCE_DIR}/plugins/graphics/render.cpp
        ${CMAKE_SOURCE_DIR}/plugins/graphics/sprite.cpp
    )
    list(APPEND PLUGIN_SOURCE ${PLUGIN_SOURCE})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/khronos)
    
    list(APPEND PLUGIN_LIBRARIES Base)

    find_package(GL REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${GL_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${GL_INCLUDE_DIR})

    list(APPEND PLUGIN_LIBRARIES GL)
    list(APPEND PLUGIN_LIBRARIES EGL)
    
    find_package(EGLPLUS REQUIRED)
    list(APPEND PLUGIN_INCLUDE_DIRS ${EGLPLUS_INCLUDE_DIR})
    find_package(OGLPLUS REQUIRED)
    list(APPEND PLUGIN_INCLUDE_DIRS ${OGLPLUS_INCLUDE_DIR})
    
    find_package(ZMQ REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})

    find_package(MSGPACK REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${MSGPACK_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIR})
    
    find_package(PNG REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${PNG_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${PNG_INCLUDE_DIR})
    
    include_directories(${PLUGIN_INCLUDE_DIRS})
    add_library(PluginGraphics SHARED ${PLUGIN_SOURCE})
    target_link_libraries(PluginGraphics ${PLUGIN_LIBRARIES})
    set_target_properties(PluginGraphics PROPERTIES PREFIX "")

    install (TARGETS PluginGraphics
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS PluginGraphics
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
