#!/bin/bash
set -e

if [ -f install/setup.bash ]; then 
    source install/setup.bash
fi

ros2 run ros_gz_image image_bridge /vertical_camera --ros-args -p transport:=compressed
