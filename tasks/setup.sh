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
        -Wall -Wextra -Wpedantic
