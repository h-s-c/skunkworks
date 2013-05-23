## Public Domain

project (PluginGraphics)
    set( PLUGIN_SOURCE
        ${CMAKE_SOURCE_DIR}/plugins/graphics/plugin.cpp
        ${CMAKE_SOURCE_DIR}/plugins/graphics/render.cpp
    )
    list(APPEND PLUGIN_SOURCE ${PLUGIN_SOURCE})
    
    list(APPEND PLUGIN_LIBRARIES Base)
    
    find_package(SDL2 REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${SDL2_LIBRARY}) 
    list(APPEND PLUGIN_INCLUDE_DIRS ${SDL2_INCLUDE_DIR}) 
      
    find_package(GLEW REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${GLEW_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${GLEW_INCLUDE_PATH})
    
    find_package(GL REQUIRED)
    list(APPEND PLUGIN_LIBRARIES ${GL_LIBRARY})   
    list(APPEND PLUGIN_INCLUDE_DIRS ${GL_INCLUDE_PATH})
    
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND PLUGIN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)
    
    include_directories(${PLUGIN_INCLUDE_DIRS} )
    add_library(PluginGraphics SHARED ${PLUGIN_SOURCE})
    target_link_libraries(PluginGraphics ${PLUGIN_LIBRARIES})
    set_target_properties(PluginGraphics PROPERTIES PREFIX "")

    install (TARGETS PluginGraphics
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS PluginGraphics
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
