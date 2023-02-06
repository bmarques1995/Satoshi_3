set CompileMode=%~1

cmake -S ./Satoshi/vendor/nlohmann_json -B ./dependencies/nlohmann_json -DCMAKE_INSTALL_PREFIX="./install" -DwxBUILD_SHARED=OFF
cmake --build ./dependencies/nlohmann_json --config %CompileMode% --target install