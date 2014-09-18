## Public Domain

project (Json++)
    set(JSONXX_SOURCE  
            ${CMAKE_SOURCE_DIR}/external/jsonxx.cpp
        )

    list(APPEND JSONXX_SOURCE ${JSONXX_SOURCE})

    list(APPEND JSONXX_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND JSONXX_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external)
    
    include_directories(${JSONXX_INCLUDE_DIRS} )
    add_library (Json++ STATIC ${JSONXX_SOURCE})
    set_target_properties(Json++ PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

project (OIS)
    set(OIS_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/OIS/OISEffect.cpp
        ${CMAKE_SOURCE_DIR}/external/OIS/OISException.cpp
        ${CMAKE_SOURCE_DIR}/external/OIS/OISForceFeedback.cpp
        ${CMAKE_SOURCE_DIR}/external/OIS/OISInputManager.cpp
        ${CMAKE_SOURCE_DIR}/external/OIS/OISJoyStick.cpp
        ${CMAKE_SOURCE_DIR}/external/OIS/OISKeyboard.cpp
        ${CMAKE_SOURCE_DIR}/external/OIS/OISObject.cpp
        )
    list(APPEND OIS_SOURCE ${OIS_SOURCE})
if(USE_SDL) 
    set(SDL_SOURCE    
            ${CMAKE_SOURCE_DIR}/external/OIS/SDL/SDLInputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/SDL/SDLKeyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/SDL/SDLJoyStick.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/SDL/SDLMouse.cpp
        )
    list(APPEND OIS_SOURCE ${SDL_SOURCE})
    find_package(SDL REQUIRED)
    list(APPEND OIS_INCLUDE_DIRS ${SDL_INCLUDE_DIR})
    list(APPEND OIS_LIBRARIES ${SDL_LIBRARY})
    add_definitions(-DOIS_SDL_PLATFORM)
elseif(USE_SDL2) 
    set(SDL2_SOURCE    
            ${CMAKE_SOURCE_DIR}/external/OIS/SDL/SDLInputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/SDL/SDLKeyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/SDL/SDLJoyStick.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/SDL/SDLMouse.cpp
        )
    list(APPEND OIS_SOURCE ${SDL2_SOURCE})
    find_package(SDL2 REQUIRED)
    list(APPEND OIS_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    list(APPEND OIS_LIBRARIES ${SDL2_LIBRARY})
    add_definitions(-DOIS_SDL2_PLATFORM)
elseif (WIN32)
    if(USE_XINPUT)
        set(WIN_SOURCE    
            ${CMAKE_SOURCE_DIR}/external/OIS/win32/Win32InputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/win32/Win32Keyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/win32/Win32JoyStick.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/win32/Win32Mouse.cpp
        )
        find_package(DirectX_ REQUIRED)
        list(APPEND OIS_INCLUDE_DIRS ${DirectX_INCLUDE_DIR})
        list(APPEND OIS_LIBRARIES ${DirectX_LIBRARIES})
    else()
        set(WIN_SOURCE    
                ${CMAKE_SOURCE_DIR}/external/OIS/win32native/Win32NativeInputManager.cpp
                ${CMAKE_SOURCE_DIR}/external/OIS/win32native/Win32NativeKeyboard.cpp
                ${CMAKE_SOURCE_DIR}/external/OIS/win32native/Win32NativeJoyStick.cpp
                ${CMAKE_SOURCE_DIR}/external/OIS/win32native/Win32NativeMouse.cpp
            )
        add_definitions(-DOIS_WIN32_NATIVE)
    endif()
    list(APPEND OIS_SOURCE ${WIN_SOURCE})
elseif (APPLE) 
    if (IPHONE)
        set(APPLE_SOURCE
            ${CMAKE_SOURCE_DIR}/external/OIS/iphone/iPhoneAccelerometer.mm
            ${CMAKE_SOURCE_DIR}/external/OIS/iphone/iPhoneInputManager.mm
            ${CMAKE_SOURCE_DIR}/external/OIS/iphone/iPhoneMultiTouch.mm
        )
    else()
        set(APPLE_SOURCE
            ${CMAKE_SOURCE_DIR}/external/OIS/mac/MacHIDManager.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/mac/MacHelpers.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/mac/MacInputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/mac/MacJoyStick.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/mac/MacKeyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/mac/MacMouse.cpp
        )
    endif()
    list(APPEND OIS_SOURCE ${APPLE_SOURCE})
    find_package(IOKit REQUIRED)
    list(APPEND OIS_LIBRARIES ${IOKit_LIBRARIES})   
elseif (UNIX)
    if (ANDROID)
        set(UNIX_SOURCE ${CMAKE_SOURCE_DIR}/external/OIS/android/AndroidInputManager.cpp)   
    else()         
        set(UNIX_SOURCE
            ${CMAKE_SOURCE_DIR}/external/OIS/linux/EventHelpers.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/linux/LinuxForceFeedback.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/linux/LinuxInputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/linux/LinuxJoyStickEvents.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/linux/LinuxKeyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/OIS/linux/LinuxMouse.cpp
        )
    endif()
    list(APPEND OIS_SOURCE ${UNIX_SOURCE})
endif()

    list(APPEND OIS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND OIS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/OIS)
    
    include_directories(${OIS_INCLUDE_DIRS} )
    add_library (OIS STATIC ${OIS_SOURCE})
    target_link_libraries(OIS ${OIS_LIBRARIES})
    set_target_properties(OIS PROPERTIES POSITION_INDEPENDENT_CODE TRUE)


project (Zeug)
    set(ZEUG_SOURCE  
            ${CMAKE_SOURCE_DIR}/external/zeug/memory_map.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/platform.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/shared_lib.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/window.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/zipreader.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/opengl/buffer.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/opengl/program.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/opengl/shader.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/opengl/texture.cpp
            ${CMAKE_SOURCE_DIR}/external/zeug/thirdparty/miniz.c
        )

    list(APPEND ZEUG_SOURCE ${ZEUG_SOURCE})

    if(UNIX)
        if(ANDROID)
            list(APPEND ZEUG_SOURCE ${CMAKE_SOURCE_DIR}/external/zeug/thirdparty/cpu-features.c)
        endif()
    endif()

    list(APPEND ZEUG_SOURCE ${ZEUG_SOURCE})

    list(APPEND ZEUG_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND ZEUG_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/zeug)
    
    include_directories(${ZEUG_INCLUDE_DIRS} )
    add_library (Zeug STATIC ${ZEUG_SOURCE})
    set_target_properties(Zeug PROPERTIES POSITION_INDEPENDENT_CODE TRUE)