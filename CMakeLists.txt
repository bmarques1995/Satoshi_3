cmake_minimum_required(VERSION 3.20)

project(Satoshi LANGUAGES CXX)

add_library(Satoshi SHARED "./Satoshi/src/include/Hello.hpp" "./Satoshi/src/lib/Hello.cpp")
if(WIN32)
target_compile_definitions(Satoshi PUBLIC ST_PLATFORM_WINDOWS)
endif()
target_compile_definitions(Satoshi PRIVATE ST_BUILD_DLL)
target_include_directories(Satoshi PRIVATE "./Satoshi/src/include/")

add_executable(Sandbox "./Sandbox/src/lib/SandboxApp.cpp")
target_include_directories(Sandbox PRIVATE "./Satoshi/src/include/" "./Sandbox/src/include/")
target_link_libraries(Sandbox PRIVATE Satoshi)