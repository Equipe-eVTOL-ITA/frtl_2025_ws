#!/bin/bash
set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 <targets>"
    exit 1
fi

if [ $1 = all ]
then
    BUILD_PATH=""
elif [ $1 = dependencies ]
then
    BUILD_PATH="--paths src/px4_msgs/* src/fsm/* src/custom_msgs src/px4_ros2_interface/*"
elif [ $1 = gesture_packages ]
then
    BUILD_PATH="--paths src/camera_publisher src/gesture_classifier"
elif [ $1 = cbr_2025 ]
then
    BUILD_PATH="--paths src/cbr_2025/*"
elif [ $1 = robocup_2025 ]
then
    BUILD_PATH="--paths src/robocup_2025/*"
elif [ $1 = sae_2025 ]
then
    BUILD_PATH="--paths src/sae_2025/*"
else
    exit 1
fi

if [ -e install/setup.bash ]
then
    source install/setup.bash
else
    source /opt/ros/humble/setup.bash
fi

# Set the default build type
BUILD_TYPE=RelWithDebInfo
colcon build \
        --symlink-install \
        --cmake-args "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" "-DCMAKE_EXPORT_COMPILE_COMMANDS=On" \
        -Wall -Wextra -Wpedantic \
        $BUILD_PATH