@echo off

pushd ..
mkdir build
cd build
cmake ..
cmake --build . 
popd
pause
