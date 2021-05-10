#!/bin/bash

# Setup network
/opt/zephyrproject_new/tools/net-tools/net-setup.sh up


#west build -b qemu_x86 samples/net/civetweb/http_server -- -DOVERLAY_CONFIG=myConf.conf

cd /opt/zephyrproject_new/dnsresolvePlayground/dnsresolve

west build -t run

/opt/zephyrproject_new/tools/net-tools/net-setup.sh down
