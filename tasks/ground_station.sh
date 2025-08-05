#!/bin/bash
set -e

# Get the workspace directory dynamically (parent of tasks directory)
WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Source the workspace setup
if [ -f "$WORKSPACE_DIR/install/setup.bash" ]; then
    echo "Sourcing workspace setup from: $WORKSPACE_DIR/install/setup.bash"
    source "$WORKSPACE_DIR/install/setup.bash"
else
    echo "Error: Workspace setup file not found at $WORKSPACE_DIR/install/setup.bash"
    echo "Please build the workspace first."
    exit 1
fi

# Check if package argument is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <package>"
    echo "Available packages: cbr_fase1, cbr_fase2, cbr_fase3, cbr_fase4"
    exit 1
fi

PACKAGE=$1

# Validate package name
case $PACKAGE in
    cbr_fase1|cbr_fase2|cbr_fase3|cbr_fase4)
        echo "Selected package: $PACKAGE"
        ;;
    *)
        echo "Error: Invalid package '$PACKAGE'"
        echo "Available packages: cbr_fase1, cbr_fase2, cbr_fase3, cbr_fase4"
        exit 1
        ;;
esac

# Launch the ROS2 package (ground station doesn't need XRCE agent)
echo "Launching ground station for package: $PACKAGE"
ros2 launch $PACKAGE ground_station.launch.py