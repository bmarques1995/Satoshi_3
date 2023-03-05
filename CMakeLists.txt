cmake_minimum_required(VERSION 3.20)

project(Satoshi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SATOSHI_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Satoshi/src/include/*.hpp")
file(GLOB_RECURSE SATOSHI_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Satoshi/src/lib/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Sandbox/src/include/*.hpp")
file(GLOB_RECURSE SANDBOX_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Sandbox/src/lib/*.cpp")

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets
    ${PROJECT_BINARY_DIR}/assets
    COMMENT "Copying shaders to binary directory"
)

set(SATOSHI_LIBS 
    spdlogd
    glad
    vulkan-1
    shaderc_combined
    jsoncpp_static
    VulkanMemoryAllocatord.lib
)

set(COMMON_LIBS

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
D3D12MAd.lib
)

set(WINDOW_ARG WIN32)
endif()

add_library(Satoshi SHARED ${SATOSHI_HEADERS} ${SATOSHI_SOURCES})
if(WIN32)
target_compile_definitions(Satoshi PUBLIC ST_PLATFORM_WINDOWS)
endif()
target_compile_definitions(Satoshi PRIVATE ST_BUILD_DLL)
target_compile_definitions(Satoshi PUBLIC _UNICODE)
target_compile_definitions(Satoshi PUBLIC WIN32_LEAN_AND_MEAN)
target_include_directories(Satoshi PRIVATE "./Satoshi/src/include/" "./install/include/" "./install/include/eigen3/")
target_link_directories(Satoshi PRIVATE "./install/lib/vc_x64_lib" "./install/lib")
target_link_libraries(Satoshi PRIVATE ${SATOSHI_LIBS} ${COMMON_LIBS})
target_precompile_headers(Satoshi PRIVATE "./Satoshi/src/include/stpch.hpp")

add_executable(Sandbox ${WINDOW_ARG} ${SANDBOX_HEADERS} ${SANDBOX_SOURCES})
target_include_directories(Sandbox PRIVATE "./Satoshi/src/include/" "./Sandbox/src/include/" "./install/include" "./install/include/eigen3/")
target_link_directories(Sandbox PRIVATE "./install/lib/vc_x64_lib" "./install/lib")
target_link_libraries(Sandbox PRIVATE Satoshi ${COMMON_LIBS})
target_precompile_headers(Sandbox PRIVATE "./Sandbox/src/include/sdbpch.hpp")
add_dependencies(Sandbox copy_resources)