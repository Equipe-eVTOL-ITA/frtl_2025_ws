# Base Image
FROM ubuntu:22.04

# Set non-interactive mode for APT
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt update && apt install -y \
    git wget curl cmake \
    gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-gl \
    libfuse2 libxcb-xinerama0 libxkbcommon-x11-0 libxcb-cursor0 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Install Python packages
RUN pip install --no-cache-dir empy==3.3.4 pyros-genmsg setuptools==59.6.0 ultralytics

# ROS 2 Humble
RUN apt update && apt install -y \
    locales && locale-gen en_US en_US.UTF-8 \
    && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
    && apt install -y software-properties-common \
    && add-apt-repository universe \
    && curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | tee /etc/apt/sources.list.d/ros2.list \
    && apt update && apt install -y ros-humble-desktop ros-dev-tools \
    && echo "source /opt/ros/humble/setup.bash" >> /etc/bash.bashrc

# Setup user for development
RUN useradd -m dev && usermod -aG sudo dev
USER dev
WORKDIR /home/dev

# MicroXRCE-Agent
RUN git clone https://github.com/eProsima/Micro-XRCE-DDS-Agent.git ~/MicroXRCE-Agent && \
    cd ~/MicroXRCE-Agent && mkdir build && cd build && cmake .. && make && make install && ldconfig

# PX4-Autopilot
RUN git clone --branch v1.15.2 --recursive https://github.com/PX4/PX4-Autopilot.git ~/PX4-Autopilot && \
    bash ~/PX4-Autopilot/Tools/setup/ubuntu.sh && \
    cd ~/PX4-Autopilot && make px4_sitl

# Grant display, camera, and network access
ENV DISPLAY=:0
