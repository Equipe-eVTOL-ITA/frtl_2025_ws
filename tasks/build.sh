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
    BUILD_PATH="--paths src/px4_msgs/* fsm/*"
elif [ $1 = custom_msgs ]
then
    BUILD_PATH="--paths src/custom_msgs"
elif [ $1 = gesture_packages ]
then
    BUILD_PATH="--paths src/camera_publisher src/gesture_classifier"
elif [ $1 = frtl_2024 ]
then
    BUILD_PATH="--paths src/frtl_2024/*"
elif [ $1 = cv_utils ]
then
    BUILD_PATH="--paths src/frtl_2024/frtl_2024_cv_utils"    
elif [ $1 = fase1 ]
then
    BUILD_PATH="--paths src/frtl_2024/frtl_2024_fase1"
elif [ $1 = fase2 ]
then
    BUILD_PATH="--paths src/frtl_2024/frtl_2024_fase2"
elif [ $1 = fase3 ]
then
    BUILD_PATH="--paths src/frtl_2024/frtl_2024_fase3"
elif [ $1 = fase4 ]
then
    BUILD_PATH="--paths src/frtl_2024/frtl_2024_fase4"
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