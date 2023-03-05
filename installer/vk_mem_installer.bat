set CompileMode=%~1

cmake -S ./Satoshi/vendor/vk_mem_alloc -B ./dependencies/vk_mem_alloc -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/vk_mem_alloc --config %CompileMode% --target install