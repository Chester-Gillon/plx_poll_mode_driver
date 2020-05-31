#! /bin/bash

mkdir -p Debug
gcc multi_platform_test.c -g -o Debug/multi_platform_test -l rt
i686-w64-mingw32-gcc multi_platform_test.c -g -o Debug/multi_platform_test.exe -static -l pthread

