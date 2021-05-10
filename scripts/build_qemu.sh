#!/bin/bash


# Switch to zephyr toolchain
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR=/opt/toolchains/zephyr-sdk-0.12.4
export ZEPHYR_BASE=/opt/zephyrproject_new/zephyr

cd /opt/zephyrproject_new/

ln -s /work/ dnsresolvePlayground

west config manifest.path dnsresolvePlayground
west update

cd dnsresolvePlayground/dnsresolve
rm -rf build/

#Note that the path changed in newer zephyr versions...
west build -b qemu_x86 -- -DOVERLAY_CONFIG=qemuOverlayConf.conf
