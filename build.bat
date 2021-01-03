REM delete output folder 
if exist build rd /s /q build
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config Release