// Public Domain

#import "framework/framework_api.hpp"

#import <Foundation/NSAutoreleasePool.h>
#import <Appkit/NSApplication.h>

static NSAutoreleasePool *pool;

static void
cocoa_init(void)
{
    pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];
}

static void
cocoa_finish(void)
{
    [pool drain];
}

int main(int argc, char* argv[])
{
    cocoa_init();
    int result = RunFramework();
    cocoa_finish();
}
