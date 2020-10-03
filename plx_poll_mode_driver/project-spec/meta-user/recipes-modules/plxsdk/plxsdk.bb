SUMMARY = "Recipe for  build an external plxsdk Linux kernel module"
SECTION = "PETALINUX/modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

INHIBIT_PACKAGE_STRIP = "1"

SRC_URI = "file://Makefile \
	   file://ApiFunc.c \
	   file://ApiFunc.h \
	   file://DrvDefs.h \
	   file://Plx.h \
	   file://PciRegs.h \
	   file://PlxChip.h \
	   file://PlxTypes.h \
	   file://PlxDefCk.h \
	   file://PlxStat.h \
	   file://PciTypes.h \
	   file://Plx_sysdep.h \
	   file://Eep_9000.h \
	   file://PciFunc.h \
	   file://PlxChipApi.h \
	   file://PlxChipFn.h \
	   file://PlxInterrupt.h \
	   file://SuppFunc.h \
	   file://Dispatch.c \
	   file://Dispatch.h \
	   file://Driver.h \
	   file://PlxIoctl.h \
	   file://Driver.c \
	   file://Eep_9000.c \
	   file://ModuleVersion.c \
	   file://PciFunc.c \
	   file://SuppFunc.c \
	   file://PlxChipApi.c \
	   file://PlxChipFn.c \
	   file://PlxInterrupt.c \
	   file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
