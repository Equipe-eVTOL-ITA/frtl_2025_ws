#!/bin/bash
set -e

if [ -f install/setup.bash ]; then 
    source install/setup.bash
fi

export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:~/PX4-Autopilot/Tools/simulation/gz/models
export GAZEBO_RESOURCE_PATH=$GAZEBO_RESOURCE_PATH:~/PX4-Autopilot/Tools/simulation/gz/worlds

cd ~/PX4-Autopilot

PX4_SYS_AUTOSTART=4001
PX4_GZ_WORLD=$1
case $1 in
    sae1)
        PX4_GZ_MODEL_POSE="0.0, 0.0, 0.05, 0.0, 0.0, 0.0"
        PX4_SIM_MODEL=x500_sae
        ;;
    sae2)
        PX4_GZ_MODEL_POSE="0.0, 0.0, 0.05, 0.0, 0.0, 0.0"
        PX4_SIM_MODEL=x500_sae
        ;;
    sae3)
        PX4_GZ_MODEL_POSE="0.0, 0.0, 0.05, 0.0, 0.0, 0.0"
        PX4_SIM_MODEL=x500_sae   
        ;;
    openlanes)
        PX4_GZ_MODEL_POSE="0.0, 0.0, 0.05, 0.0, 0.0, 0.0"
        PX4_SIM_MODEL=x500_tdp
        ;;
    fase1_25)
        PX4_GZ_MODEL_POSE="8.0, 2.0, 0.6, 0.0, 0.0, 1.57"
        PX4_SIM_MODEL=x500_simulation
        ;;
    fase2_25)
        PX4_GZ_MODEL_POSE="8.0, 2.0, 0.6, 0.0, 0.0, 1.57"
        PX4_SIM_MODEL=x500_simulation
        ;;
    fase3)
        PX4_GZ_MODEL_POSE="8.0, 2.0, 0.6, 0.0, 0.0, 1.57"
        PX4_SIM_MODEL=x500_simulation
        ;;
    fase4_25)
        PX4_GZ_MODEL_POSE="8.0, 2.0, 0.6, 0.0, 0.0, 1.57"
        PX4_SIM_MODEL=x500_simulation
        ;;
    *)
        PX4_GZ_MODEL_POSE="0.0, 0.0, 0.0, 0.0, 0.0, 0.0"
        PX4_SIM_MODEL=x500_simulation
        ;;
esac

#Multi-vehicles: usar script simulation-gazebo - descomentar as duas linhas

#python3 Tools/simulation/gz/simulation-gazebo --world $PX4_GZ_WORLD &

#PX4_GZ_STANDALONE=1 \
PX4_SYS_AUTOSTART=$PX4_SYS_AUTOSTART \
PX4_GZ_MODEL_POSE=$PX4_GZ_MODEL_POSE \
PX4_GZ_WORLD=$PX4_GZ_WORLD \
PX4_SIM_MODEL=$PX4_SIM_MODEL \
./build/px4_sitl_default/bin/px4
