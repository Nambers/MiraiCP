#!/bin/bash
echo "--- start cppBuilds.sh ---"
echo "--- preBuild ---"
workDir="$(pwd)/"
echo "working directory: $workDir"

# if $1 is "clean", then clean build directory



if [ "$1" = "clean" ]; then
    echo "--- clean build directory ---"
    if [ -d $workDir"cpp/build" ]; then
        rm -rf $workDir"cpp/build"
    else
        echo "Warning: build directory not found"
    fi
    exit 0
fi

if [ ! -d $workDir"cpp/build" ]; then
    mkdir -p $workDir"cpp/build"
fi

cd $workDir"cpp/build"
if ! cmake -DCMAKE_BUILD_TYPE=Release -DGOOGLE_TEST=OFF ..
then
    echo "Error: cmake failed"
    exit 1
fi

case $1 in
    multi)
        echo "build multi"
        buildType="MiraiCP_multi"
        ;;

    single)
        echo "build single"
        buildType="MiraiCP_single"
        ;;

    libLoader)
        echo "build libLoader"
        buildType="Loader"
        ;;
    
    libLoaderNative)
        echo "build libLoaderNative"
        buildType="LoaderNative"
        ;;

    *)
        echo "Error: unknown build type"
        exit 1
        ;;
esac

if ! make $buildType -j6
then
    echo "Error: make failed"
    exit 1
fi

echo "--- end cppBuilds.sh ---"
exit 0