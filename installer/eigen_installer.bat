set CompileMode=%~1

cmake -S ./Satoshi/vendor/eigen3 -B ./dependencies/eigen3 -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/eigen3 --config %CompileMode% --target install