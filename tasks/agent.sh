#!/bin/bash
set -e

if [ -f install/setup.bash ]; then source install/setup.bash; fi
export PATH=$PATH:~/Micro-XRCE-DDS-Agent/build
MicroXRCEAgent udp4 -p 8888