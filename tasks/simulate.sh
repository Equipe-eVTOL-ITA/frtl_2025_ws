#!/bin/bash
set -e

if [ -f install/setup.bash ]; then 
    source install/setup.bash
fi

export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:/home/ceccon/PX4-Autopilot/Tools/simulation/gz/models
export GAZEBO_RESOURCE_PATH=$GAZEBO_RESOURCE_PATH:/usr/share/gazebo-11

cd ~/PX4-Autopilot

PX4_SYS_AUTOSTART=4001
PX4_GZ_WORLD=$1
PX4_GZ_MODEL=x500_simulation
case $1 in
    openlanes)
        PX4_GZ_MODEL_POSE="0.0, 0.0, 0.0, 0.0, 0.0, 0.0"
        ;;
    fase1)
        PX4_GZ_MODEL_POSE="8.0, 2.2, 0.6, 0.0, 0.0, 1.57"
        ;;
    fase2)
        PX4_GZ_MODEL_POSE="8.0, 2.2, 0.6, 0.0, 0.0, 1.57"
        ;;
    fase3)
        PX4_GZ_MODEL_POSE="8.0, 2.2, 0.6, 0.0, 0.0, 1.57"
        ;;
    fase4)
        PX4_GZ_MODEL_POSE="8.0, 2.2, 0.6, 0.0, 0.0, 1.57"
        ;;
    *)
        PX4_GZ_MODEL_POSE="0.0, 0.0, 0.0, 0.0, 0.0, 0.0"
        ;;
esac


PX4_SYS_AUTOSTART=$PX4_SYS_AUTOSTART \
PX4_GZ_MODEL_POSE=$PX4_GZ_MODEL_POSE \
PX4_GZ_WORLD=$PX4_GZ_WORLD \
PX4_GZ_MODEL=x500_simulation \
./build/px4_sitl_default/bin/px4