@echo off

call ./installer/spdlog_installer Release
call ./installer/glad_installer Release
call ./installer/vulkan_installer Release
call ./installer/eigen_installer Release
call ./installer/jsoncpp_installer Release
call ./installer/shaderc_installer Release