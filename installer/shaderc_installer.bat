set CompileMode=%~1

cmake -S ./Satoshi/vendor/shaderc_wrapper -B ./dependencies/shaderc_wrapper -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/shaderc_wrapper --config %CompileMode% --target install