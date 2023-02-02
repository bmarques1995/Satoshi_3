set CompileMode=%~1

cmake -S ./Satoshi/vendor/glad -B ./dependencies/glad -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/glad --config %CompileMode% --target install