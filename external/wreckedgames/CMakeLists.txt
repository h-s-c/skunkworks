## Public Domain

project (OIS)
    set(OIS_SOURCE    
        ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/OISEffect.cpp
        ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/OISException.cpp
        ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/OISForceFeedback.cpp
        ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/OISInputManager.cpp
        ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/OISJoyStick.cpp
        ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/OISKeyboard.cpp
        ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/OISObject.cpp
        )
    list(APPEND OIS_SOURCE ${OIS_SOURCE})
if(USE_SDL) 
    set(SDL_SOURCE    
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/SDL/SDLInputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/SDL/SDLKeyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/SDL/SDLJoyStick.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/SDL/SDLMouse.cpp
        )
    list(APPEND OIS_SOURCE ${SDL_SOURCE})
    find_package(SDL REQUIRED)
    list(APPEND OIS_INCLUDE_DIRS ${SDL_INCLUDE_DIR})
    list(APPEND OIS_LIBRARIES ${SDL_LIBRARY})
    add_definitions(-DOIS_SDL_PLATFORM)
elseif(USE_SDL2) 
    set(SDL2_SOURCE    
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/SDL/SDLInputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/SDL/SDLKeyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/SDL/SDLJoyStick.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/SDL/SDLMouse.cpp
        )
    list(APPEND OIS_SOURCE ${SDL2_SOURCE})
    find_package(SDL2 REQUIRED)
    list(APPEND OIS_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    list(APPEND OIS_LIBRARIES ${SDL2_LIBRARY})
    add_definitions(-DOIS_SDL2_PLATFORM)
elseif (WIN32)
    if(USE_XINPUT)
        set(WIN_SOURCE    
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/win32/Win32InputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/win32/Win32Keyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/win32/Win32JoyStick.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/win32/Win32Mouse.cpp
        )
        find_package(DirectX_ REQUIRED)
        list(APPEND OIS_INCLUDE_DIRS ${DirectX_INCLUDE_DIR})
        list(APPEND OIS_LIBRARIES ${DirectX_LIBRARIES})
    else()
        set(WIN_SOURCE    
                ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/win32native/Win32NativeInputManager.cpp
                ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/win32native/Win32NativeKeyboard.cpp
                ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/win32native/Win32NativeJoyStick.cpp
                ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/win32native/Win32NativeMouse.cpp
            )
        add_definitions(-DOIS_WIN32_NATIVE)
    endif()
    list(APPEND OIS_SOURCE ${WIN_SOURCE})
elseif (APPLE) 
    if (IPHONE)
        set(APPLE_SOURCE
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/iphone/iPhoneAccelerometer.mm
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/iphone/iPhoneInputManager.mm
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/iphone/iPhoneMultiTouch.mm
        )
    else()
        set(APPLE_SOURCE
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/mac/MacHIDManager.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/mac/MacHelpers.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/mac/MacInputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/mac/MacJoyStick.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/mac/MacKeyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/mac/MacMouse.cpp
        )
    endif()
    list(APPEND OIS_SOURCE ${APPLE_SOURCE})
    find_package(IOKit REQUIRED)
    list(APPEND OIS_LIBRARIES ${IOKit_LIBRARIES})   
elseif (UNIX)
    if (ANDROID)
        set(UNIX_SOURCE ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/android/AndroidInputManager.cpp)   
    else()         
        set(UNIX_SOURCE
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/linux/EventHelpers.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/linux/LinuxForceFeedback.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/linux/LinuxInputManager.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/linux/LinuxJoyStickEvents.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/linux/LinuxKeyboard.cpp
            ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS/linux/LinuxMouse.cpp
        )
    endif()
    list(APPEND OIS_SOURCE ${UNIX_SOURCE})
endif()

    list(APPEND OIS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND OIS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/wreckedgames/OIS)
    
    add_definitions(-DOIS_DYNAMIC_LIB -DOIS_NONCLIENT_BUILD)
    include_directories(${OIS_INCLUDE_DIRS} )
    add_library (OIS SHARED ${OIS_SOURCE})
    target_link_libraries(OIS ${OIS_LIBRARIES})
    
    install (TARGETS OIS
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS OIS
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
