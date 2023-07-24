@echo off

mkdir build

pushd build
cmake -A x64 .. || exit /b
cmake --build . --config Release || exit /b
popd

build\Release\voyx.exe %*
