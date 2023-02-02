cmake_minimum_required(VERSION 3.20)

project(Satoshi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SATOSHI_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Satoshi/src/include/*.hpp")
file(GLOB_RECURSE SATOSHI_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Satoshi/src/lib/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Sandbox/src/include/*.hpp")
file(GLOB_RECURSE SANDBOX_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Sandbox/src/lib/*.cpp")

set(SATOSHI_LIBS 
    spdlogd
)

set(WINDOW_ARG)

if(WIN32)
set(SATOSHI_LIBS
${SATOSHI_LIBS}
d3d11
d3d12
dxgi
dxguid
d3dcompiler
opengl32
)

set(WINDOW_ARG WIN32)
endif()

add_library(Satoshi SHARED ${SATOSHI_HEADERS} ${SATOSHI_SOURCES})
if(WIN32)
target_compile_definitions(Satoshi PUBLIC ST_PLATFORM_WINDOWS)
endif()
target_compile_definitions(Satoshi PRIVATE ST_BUILD_DLL)
target_include_directories(Satoshi PRIVATE "./Satoshi/src/include/" "./install/include/")
target_link_directories(Satoshi PRIVATE "./install/lib")
target_link_libraries(Satoshi PRIVATE ${SATOSHI_LIBS})

add_executable(Sandbox ${WINDOW_ARG} ${SANDBOX_HEADERS} ${SANDBOX_SOURCES})
target_include_directories(Sandbox PRIVATE "./Satoshi/src/include/" "./Sandbox/src/include/" "./install/include")
target_link_libraries(Sandbox PRIVATE Satoshi)