set CompileMode=%~1

cmake -S ./Satoshi/vendor/jsoncpp -B ./dependencies/jsoncpp -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/jsoncpp --config %CompileMode% --target install