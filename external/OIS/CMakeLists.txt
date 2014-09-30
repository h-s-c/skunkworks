project (OIS)
    set(OIS_SOURCE    
        ${CMAKE_CURRENT_LIST_DIR}/OISEffect.cpp
        ${CMAKE_CURRENT_LIST_DIR}/OISException.cpp
        ${CMAKE_CURRENT_LIST_DIR}/OISForceFeedback.cpp
        ${CMAKE_CURRENT_LIST_DIR}/OISInputManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/OISJoyStick.cpp
        ${CMAKE_CURRENT_LIST_DIR}/OISKeyboard.cpp
        ${CMAKE_CURRENT_LIST_DIR}/OISObject.cpp
        )
    list(APPEND OIS_SOURCE ${OIS_SOURCE})
if(USE_SDL) 
    set(SDL_SOURCE    
            ${CMAKE_CURRENT_LIST_DIR}/SDL/SDLInputManager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/SDL/SDLKeyboard.cpp
            ${CMAKE_CURRENT_LIST_DIR}/SDL/SDLJoyStick.cpp
            ${CMAKE_CURRENT_LIST_DIR}/SDL/SDLMouse.cpp
        )
    list(APPEND OIS_SOURCE ${SDL_SOURCE})
    find_package(SDL REQUIRED)
    list(APPEND OIS_INCLUDE_DIRS ${SDL_INCLUDE_DIR})
    list(APPEND OIS_LIBRARIES ${SDL_LIBRARY})
    add_definitions(-DOIS_SDL_PLATFORM)
elseif(USE_SDL2) 
    set(SDL2_SOURCE    
            ${CMAKE_CURRENT_LIST_DIR}/SDL/SDLInputManager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/SDL/SDLKeyboard.cpp
            ${CMAKE_CURRENT_LIST_DIR}/SDL/SDLJoyStick.cpp
            ${CMAKE_CURRENT_LIST_DIR}/SDL/SDLMouse.cpp
        )
    list(APPEND OIS_SOURCE ${SDL2_SOURCE})
    find_package(SDL2 REQUIRED)
    list(APPEND OIS_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    list(APPEND OIS_LIBRARIES ${SDL2_LIBRARY})
    add_definitions(-DOIS_SDL2_PLATFORM)
elseif (WIN32)
    if(USE_XINPUT)
        set(WIN_SOURCE    
            ${CMAKE_CURRENT_LIST_DIR}/win32/Win32InputManager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/win32/Win32Keyboard.cpp
            ${CMAKE_CURRENT_LIST_DIR}/win32/Win32JoyStick.cpp
            ${CMAKE_CURRENT_LIST_DIR}/win32/Win32Mouse.cpp
        )
        find_package(DirectX_ REQUIRED)
        list(APPEND OIS_INCLUDE_DIRS ${DirectX_INCLUDE_DIR})
        list(APPEND OIS_LIBRARIES ${DirectX_LIBRARIES})
    else()
        set(WIN_SOURCE    
                ${CMAKE_CURRENT_LIST_DIR}/win32native/Win32NativeInputManager.cpp
                ${CMAKE_CURRENT_LIST_DIR}/win32native/Win32NativeKeyboard.cpp
                ${CMAKE_CURRENT_LIST_DIR}/win32native/Win32NativeJoyStick.cpp
                ${CMAKE_CURRENT_LIST_DIR}/win32native/Win32NativeMouse.cpp
            )
        add_definitions(-DOIS_WIN32_NATIVE)
    endif()
    list(APPEND OIS_SOURCE ${WIN_SOURCE})
elseif (APPLE) 
    if (IPHONE)
        set(APPLE_SOURCE
            ${CMAKE_CURRENT_LIST_DIR}/iphone/iPhoneAccelerometer.mm
            ${CMAKE_CURRENT_LIST_DIR}/iphone/iPhoneInputManager.mm
            ${CMAKE_CURRENT_LIST_DIR}/iphone/iPhoneMultiTouch.mm
        )
    else()
        set(APPLE_SOURCE
            ${CMAKE_CURRENT_LIST_DIR}/mac/MacHIDManager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/mac/MacHelpers.cpp
            ${CMAKE_CURRENT_LIST_DIR}/mac/MacInputManager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/mac/MacJoyStick.cpp
            ${CMAKE_CURRENT_LIST_DIR}/mac/MacKeyboard.cpp
            ${CMAKE_CURRENT_LIST_DIR}/mac/MacMouse.cpp
        )
    endif()
    list(APPEND OIS_SOURCE ${APPLE_SOURCE})
    find_package(IOKit REQUIRED)
    list(APPEND OIS_LIBRARIES ${IOKit_LIBRARIES})   
elseif (UNIX)
    if (ANDROID)
        set(UNIX_SOURCE ${CMAKE_CURRENT_LIST_DIR}/android/AndroidInputManager.cpp)   
    else()         
        set(UNIX_SOURCE
            ${CMAKE_CURRENT_LIST_DIR}/linux/EventHelpers.cpp
            ${CMAKE_CURRENT_LIST_DIR}/linux/LinuxForceFeedback.cpp
            ${CMAKE_CURRENT_LIST_DIR}/linux/LinuxInputManager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/linux/LinuxJoyStickEvents.cpp
            ${CMAKE_CURRENT_LIST_DIR}/linux/LinuxKeyboard.cpp
            ${CMAKE_CURRENT_LIST_DIR}/linux/LinuxMouse.cpp
        )
    endif()
    list(APPEND OIS_SOURCE ${UNIX_SOURCE})
endif()

    list(APPEND OIS_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR})
    list(APPEND OIS_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/..)
    
    include_directories(${OIS_INCLUDE_DIRS} )
    add_library (OIS STATIC ${OIS_SOURCE})
    target_link_libraries(OIS ${OIS_LIBRARIES})
    set_target_properties(OIS PROPERTIES POSITION_INDEPENDENT_CODE TRUE)