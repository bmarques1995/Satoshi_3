set CompileMode=%~1

cmake -S ./Satoshi/vendor/wx_widgets -B ./dependencies/wx_widgets -DCMAKE_INSTALL_PREFIX="./install" -DwxBUILD_SHARED=OFF
cmake --build ./dependencies/wx_widgets --config %CompileMode% --target install