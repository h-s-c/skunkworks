// Public Domain

#include "external/android_native_app_glue.h"
#include "framework/framework_api.hpp"

static void custom_handle_cmd(struct android_app* app, int32_t cmd) {
    switch(cmd) {
       case APP_CMD_INIT_WINDOW:
        RunFramework(app->window);
        break;
    }
}

void android_main(struct android_app* state) 
{
    app_dummy();
    int events;
    state->onAppCmd = custom_handle_cmd;

    while (1) {
        struct android_poll_source* source;
        while (ALooper_pollAll(-1, NULL, &events, (void**)&source) >= 0) {
            if (source != NULL) {
                source->process(state, source);
            }
            if (state->destroyRequested != 0) {
                return;
            }
        }
    }
}
