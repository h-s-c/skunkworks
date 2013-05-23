## Public Domain

project (Framework)
    set( FRAMEWORK_SOURCE
        ${CMAKE_SOURCE_DIR}/framework/framework.cpp
    )
    list(APPEND FRAMEWORK_SOURCE ${FRAMEWORK_SOURCE})
    
    list(APPEND FRAMEWORK_LIBRARIES Base)
    
    find_package(SDL2 REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${SDL2_LIBRARY}) 
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${SDL2_INCLUDE_DIR}) 
      
    find_package(GLEW REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${GLEW_LIBRARY})   
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${GLEW_INCLUDE_PATH})
    
    find_package(GL REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${GL_LIBRARY})   
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${GL_INCLUDE_PATH})
    
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)
    
    include_directories(${FRAMEWORK_INCLUDE_DIRS} )
    link_directories(${FRAMEWORK_LIBRARY_DIRS} )
    add_library (Framework ${FRAMEWORK_SOURCE})
    target_link_libraries(Framework ${FRAMEWORK_LIBRARIES})
