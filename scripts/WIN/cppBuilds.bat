@echo off
echo --- start cppBuilds.bat ---
echo --- prebuild ---
set workDir=%__CD__%
echo working directory: %workDir%
IF "$1"=="clean" (
    echo --- clean build directory ---
    IF exist %workDir%cpp\build\ (
        rmdir /s /q %workDir%cpp\build
    ) ELSE (
        echo warning: build directory not found
    )
    exit /b 0
)

IF not exist %workDir%cpp\build\ (
    mkdir %workDir%cpp\build
)
cd %workDir%cpp\build

IF "%2"=="mingw" (
    echo --- build with MinGW ---
    :: mingw
    cmake -G "CodeBlocks - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DGOOGLE_TEST=OFF ..
) ELSE ( 
    echo --- build with default CMake provider ---
    cmake -DCMAKE_BUILD_TYPE=Release -DGOOGLE_TEST=OFF ..
)

IF errorlevel 1 (
    echo Error: cmake failed
    exit /b 1
)

IF "%1"=="multi" (
    set targetName="MiraiCP_multi"
    goto build
)
IF "%1"=="single" (
    set targetName="MiraiCP_single"
    goto build
)
IF "%1"=="libLoader" (
    set targetName="Loader"
    goto build
)
IF "%1"=="libLoaderNative" (
    set targetName="LoaderNative"
    goto build
)

echo Error: parameter error
cd %workDir%
exit /b 1

:build
echo --- build %targetName ---
cd %workDir%cpp
@echo on
cmake --build build --target %targetName% --config Release
@echo off

:end
echo --- end cppBuilds.bat ---
:: restore working directory
cd %workDir%

exit /b 0