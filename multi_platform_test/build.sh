#! /bin/bash

# Compile the source in this project
mkdir -p Debug
gcc test_lib.c multi_platform_test.c -g -o Debug/multi_platform_test -l rt
i686-w64-mingw32-gcc test_lib.c multi_platform_test.c -g -o Debug/multi_platform_test.exe -static -l pthread

# Compile libxml2 examples.
# For the Windows executables static libraries are used for those from mingw, which are not standard Windows DLLs.
# LIBXML_STATIC is required to avoid getting unresolved errors for xmlFree (a global function pointer rather an actual function)
# when linking against the static libxml2 library.
examples_root=/usr/share/doc/libxml2-devel-2.7.6/examples
if [ ! -d ${examples_root} ]
then
    examples_root=/usr/share/doc/libxml2-doc/examples
fi
if [ ! -d ${examples_root} ]
then
    examples_root=/usr/share/doc/libxml2-devel/examples
fi
for example_name in io1 io2 reader1 reader2 reader3 reader4 testWriter tree1 tree2 xpath1 xpath2
do
    gcc ${examples_root}/${example_name}.c -I /usr/include/libxml2 -g -o Debug/${example_name} -l xml2
    i686-w64-mingw32-gcc -DLIBXML_STATIC -I '=/mingw/include/libxml2' ${examples_root}/${example_name}.c \
        -g -o Debug/${example_name}.exe -Wl,-Bstatic -l xml2  -l iconv -l z -Wl,-Bdynamic -l ws2_32
done

# Save the XML example source code (with test instructions), input file and expected output files.
mkdir -p Debug/xml_examples
cp -p ${examples_root}/* Debug/xml_examples

