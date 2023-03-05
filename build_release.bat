@echo off

call ./installer/spdlog_installer Release
call ./installer/glad_installer Release
call ./installer/vulkan_installer Release
call ./installer/eigen_installer Release
call ./installer/jsoncpp_installer Release
call ./installer/shaderc_installer Release
call ./installer/vk_mem_installer Release
call ./installer/d3d12_mem_installer Release