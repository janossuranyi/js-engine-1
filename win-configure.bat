set SDL2DIR=d:/lib/SDL2
set ASSIMP_DIR=d:/lib/assimp

md ..\js-engine-1-build
cd ..\js-engine-1-build
cmake -L ..\js-engine-1
REM cmake --build . --config Debug

pause
