// Public Domain

#include "framework/framework_api.hpp"

int main(int argc, char* argv[])
{
    return RunFramework();
}

#ifdef ANDROID
#include <thread>
#include <android/native_activity.h>
// We us these callbacks in our zeug utility code
extern void onStart(ANativeActivity* activity);
extern void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window);
extern "C" void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize)
{
    activity->callbacks->onStart = onStart;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    std::thread thread([&]
    {  
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
         main(0, NULL);  
#pragma GCC diagnostic pop
    });
    thread.detach();
}
#endif