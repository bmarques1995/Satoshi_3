set CompileMode=%~1

cmake -S ./Satoshi/vendor/spdlog -B ./dependencies/spdlog -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/spdlog --config %CompileMode% --target install