#!/bin/bash
set -e

# Define o diretório do workspace
WORKSPACE_DIR=~/frtl_2025_ws
SRC_DIR=$WORKSPACE_DIR/src

# Cria os diretórios se não existirem
cd $WORKSPACE_DIR
mkdir -p $SRC_DIR

# Instala dependências do ROS e PX4
sudo apt install ros-humble-vision-msgs

#px4_msgs
if [ ! -d "$SRC_DIR/px4_msgs" ]; then
    git clone --branch release/1.15 --recursive https://github.com/PX4/px4_msgs.git $SRC_DIR/px4_msgs
else
    echo "px4_msgs directory already exists. Skipping clone."
fi

#px4-ros2-interface-lib
if [ ! -d "$SRC_DIR/px4_ros2_interface" ]; then
    git clone --branch 1.3.0 --recursive https://github.com/Auterion/px4-ros2-interface-lib.git $SRC_DIR/px4_ros2_interface
else
    echo "px4_ros2_interface directory already exists. Skipping clone."
fi

#fsm
if [ ! -d "$SRC_DIR/fsm" ]; then
    git clone --branch main https://github.com/Equipe-eVTOL-ITA/fsm.git $SRC_DIR/fsm
else
    echo "fsm directory already exists. Skipping clone."
fi

#cbr_2025
if [ ! -d "$SRC_DIR/cbr_2025" ]; then
    git clone https://github.com/Equipe-eVTOL-ITA/cbr_2025.git $SRC_DIR/cbr_2025
else
    echo "cbr_2025 directory already exists. Skipping clone."
fi

#robocup_2025
if [ ! -d "$SRC_DIR/robocup_2025" ]; then
    git clone https://github.com/Equipe-eVTOL-ITA/robocup_2025.git $SRC_DIR/robocup_2025
else
    echo "robocup_2025 directory already exists. Skipping clone."
fi

#itajuba
if [ ! -d "$SRC_DIR/itajuba" ]; then
    git clone https://github.com/Equipe-eVTOL-ITA/itajuba.git $SRC_DIR/itajuba
else
    echo "itajuba directory already exists. Skipping clone."
fi

#camera_publisher
if [ ! -d "$SRC_DIR/camera_publisher" ]; then
    git clone https://github.com/Equipe-eVTOL-ITA/camera_publisher.git $SRC_DIR/camera_publisher
else
    echo "camera_publisher directory already exists. Skipping clone."
fi

#gesture_classifier
if [ ! -d "$SRC_DIR/gesture_classifier" ]; then
    git clone https://github.com/Equipe-eVTOL-ITA/gesture_classifier.git $SRC_DIR/gesture_classifier
else
    echo "gesture_classifier directory already exists. Skipping clone."
fi

#custom_msgs
if [ ! -d "$SRC_DIR/custom_msgs" ]; then
    git clone https://github.com/Equipe-eVTOL-ITA/custom_msgs.git $SRC_DIR/custom_msgs
else
    echo "custom_msgs directory already exists. Skipping clone."
fi

# Configura e inicializa os submódulos
if [ ! -f "$WORKSPACE_DIR/.gitmodules" ]; then
    echo "Erro: Arquivo .gitmodules não encontrado. Certifique-se de que este repositório foi clonado corretamente."
    exit 1
fi

echo "Inicializando submódulos..."
git submodule update --init --recursive

# ROS2 Humble <--> Gazebo Garden communication
sudo apt-get remove -y ros-humble-ros-gz*
sudo apt-get install -y ros-humble-ros-gzgarden

# Compila o workspace
cd $WORKSPACE_DIR
source /opt/ros/humble/setup.bash

BUILD_TYPE=RelWithDebInfo
colcon build \
    --symlink-install \
    --event-handlers console_direct+ \
    --cmake-args "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" "-DCMAKE_EXPORT_COMPILE_COMMANDS=On" \
    --executor sequential \
