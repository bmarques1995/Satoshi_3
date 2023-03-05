set CompileMode=%~1

cmake -S ./Satoshi/vendor/d3d12_mem_alloc -B ./dependencies/d3d12_mem_alloc -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/d3d12_mem_alloc --config %CompileMode% --target install