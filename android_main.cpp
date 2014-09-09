// Public Domain

#include "external/android_native_app_glue.h"
#include "framework/framework_api.hpp"

void android_main(struct android_app* state) 
{
    app_dummy();
    RunFramework();
}
