#! /bin/bash

TOOLS_ROOT=/opt/Xilinx/Vitis/2020.1/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin
PLX_SDK_ROOT=${HOME}/gsc/PlxSdk-8.00/PlxSdk

mkdir -p Debug_aarch32
${TOOLS_ROOT}/arm-linux-gnueabihf-gcc -DPLX_LINUX -DPLX_LITTLE_ENDIAN -I${PLX_SDK_ROOT}/Include -ldl -lm \
   -o Debug_aarch32/plx_poll_mode_driver plx_poll_mode_driver.c \
   ${PLX_SDK_ROOT}/PlxApi/PlxApi.c ${PLX_SDK_ROOT}/PlxApi/SdbComPort.c ${PLX_SDK_ROOT}/PlxApi/PlxApiDirect.c \
   ${PLX_SDK_ROOT}/PlxApi/I2cAaUsb.c ${PLX_SDK_ROOT}/PlxApi/Aardvark.c ${PLX_SDK_ROOT}/PlxApi/Eep_8000.c \
   ${PLX_SDK_ROOT}/PlxApi/MdioSpliceUsb.c
