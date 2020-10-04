#! /bin/bash

TOOLS_ROOT=/opt/Xilinx/Vitis/2020.1/gnu/aarch64/lin/aarch64-linux/bin
PLX_SDK_ROOT=${HOME}/gsc/PlxSdk-8.00/PlxSdk

mkdir -p Debug_aarch64
${TOOLS_ROOT}/aarch64-linux-gnu-gcc -DPLX_LINUX -DPLX_LITTLE_ENDIAN -I${PLX_SDK_ROOT}/Include -ldl -lm \
   -o Debug_aarch64/plx_poll_mode_driver plx_poll_mode_driver.c \
   ${PLX_SDK_ROOT}/PlxApi/PlxApi.c ${PLX_SDK_ROOT}/PlxApi/SdbComPort.c ${PLX_SDK_ROOT}/PlxApi/PlxApiDirect.c \
   ${PLX_SDK_ROOT}/PlxApi/I2cAaUsb.c ${PLX_SDK_ROOT}/PlxApi/Aardvark.c ${PLX_SDK_ROOT}/PlxApi/Eep_8000.c \
   ${PLX_SDK_ROOT}/PlxApi/MdioSpliceUsb.c

# The aarch64-linux-gnu-gcc 9.2.0 supplied with the Xilinx 2020.1 software doesn't support Ada; attempting to compile
# an Ada program failed with:
#    aarch64-xilinx-linux-gcc.real: error: hello_world.adb: Ada compiler not installed on this system
#
# The Ubuntu 18.04.5 LTS has the gnat-8-aarch64-linux-gnu package with gcc 8.4.0
# Ubuntu 20.04 looks to have later versions for gcc 9 and 10.
aarch64-linux-gnu-gnatmake hello_world.adb --subdirs=Debug_aarch64
