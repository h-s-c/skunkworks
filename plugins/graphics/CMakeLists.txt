## Public Domain

project (PluginGraphics)
    set( PLUGIN_SOURCE
        ${CMAKE_SOURCE_DIR}/plugins/graphics/plugin.cpp
        ${CMAKE_SOURCE_DIR}/plugins/graphics/render.cpp
        ${CMAKE_SOURCE_DIR}/plugins/graphics/sprite.cpp
    )
    list(APPEND PLUGIN_SOURCE ${PLUGIN_SOURCE})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)
    
    list(APPEND PLUGIN_LIBRARIES Base)

    find_package(EGL REQUIRED)
    list(APPEND PLUGIN_INCLUDE_DIRS ${EGL_INCLUDE_DIR})
    list(APPEND PLUGIN_LIBRARIES ${EGL_LIBRARY})   
    find_package(GLES3 REQUIRED)
    list(APPEND PLUGIN_INCLUDE_DIRS ${GLES3_INCLUDE_DIR})
    list(APPEND PLUGIN_LIBRARIES ${GLES3_LIBRARY})  

    if (UNIX)
        list(APPEND PLUGIN_LIBRARIES ${CMAKE_DL_LIBS})

        find_package(X11 REQUIRED)
        list(APPEND PLUGIN_LIBRARIES ${X11_LIBRARY})   
        list(APPEND PLUGIN_INCLUDE_DIRS ${X11_INCLUDE_DIR})
    endif()

    find_package(PLATT REQUIRED)
    list(APPEND PLUGIN_INCLUDE_DIRS ${PLATT_INCLUDE_DIR})
    
    find_package(ZMQ REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${ZMQ_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR})

    find_package(MSGPACK REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${MSGPACK_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIR})
    
    include_directories(${PLUGIN_INCLUDE_DIRS})
    add_library(PluginGraphics SHARED ${PLUGIN_SOURCE})
    target_link_libraries(PluginGraphics ${PLUGIN_LIBRARIES})
    set_target_properties(PluginGraphics PROPERTIES PREFIX "")