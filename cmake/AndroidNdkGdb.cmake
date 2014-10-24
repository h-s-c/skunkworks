# Copyright (c) 2014, Pavel Rojtberg
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# add gdbserver and general gdb configuration to project
# also create a mininal NDK skeleton so ndk-gdb finds the paths
macro(android_ndk_gdb_enable)
    if(ANDROID)
        # create custom target that depends on the real target so it gets executed afterwards
        add_custom_target(NDK_GDB ALL)
        
        set(NDK_GDB_SOLIB_PATH ${CMAKE_BINARY_DIR}/obj/local/${ANDROID_NDK_ABI_NAME}/)
        file(MAKE_DIRECTORY ${NDK_GDB_SOLIB_PATH})
        
        # 1. generate essential Android Makefiles
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/jni)
        if(NOT EXISTS ${CMAKE_BINARY_DIR}/jni/Android.mk)
            file(WRITE ${CMAKE_BINARY_DIR}/jni/Android.mk "APP_ABI := ${ANDROID_NDK_ABI_NAME}\n")
        endif()
        if(NOT EXISTS ${CMAKE_BINARY_DIR}/jni/Application.mk)
            file(WRITE ${CMAKE_BINARY_DIR}/jni/Application.mk "APP_ABI := ${ANDROID_NDK_ABI_NAME}\n")
        endif()
    
        # 2. generate gdb.setup
        get_directory_property(PROJECT_INCLUDES DIRECTORY ${PROJECT_SOURCE_DIR} INCLUDE_DIRECTORIES)
        string(REGEX REPLACE ";" " " PROJECT_INCLUDES "${PROJECT_INCLUDES}")
        file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gdb.setup "set solib-search-path ${NDK_GDB_SOLIB_PATH}\n")
        file(APPEND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gdb.setup "directory ${PROJECT_INCLUDES}\n")
    
        # 3. copy gdbserver executable
        file(COPY ${PROJECT_SOURCE_DIR}/../toolchains/android-armv7a-gcc-libstdc++/prebuilt/android-${ANDROID_ARCH_NAME}/gdbserver/gdbserver DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()
endmacro()

# register a target for remote debugging
# copies the debug version to NDK_GDB_SOLIB_PATH then strips symbols of original
macro(android_ndk_gdb_debuggable TARGET_NAME)
    if(ANDROID)
        get_property(TARGET_LOCATION TARGET ${TARGET_NAME} PROPERTY LOCATION)
        
        # create custom target that depends on the real target so it gets executed afterwards
        add_dependencies(NDK_GDB ${TARGET_NAME})
    
        # 4. copy lib to obj
        add_custom_command(TARGET NDK_GDB POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TARGET_LOCATION} ${NDK_GDB_SOLIB_PATH})
    
        # 5. strip symbols
        add_custom_command(TARGET NDK_GDB POST_BUILD COMMAND ${CMAKE_STRIP} ${TARGET_LOCATION})
    endif()
endmacro()