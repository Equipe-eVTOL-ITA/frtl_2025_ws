# a principal funcao desse arquivo é automatizar o processo de iniciar vários nodes
# e configurar seus parâmetros de uma só vez

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument, TimerAction
from launch.substitutions import LaunchConfiguration
import os
from ament_index_python.packages import get_package_share_directory

# esta é a função que o ros2 launch procura
def generate_launch_description():
    pkg = get_package_share_directory('itajuba_fase4')
    params = os.path.join(pkg, "launch", "params.yaml") # aqui explica como os parâmetros em yaml chegam no código
    #rviz_cfg = os.path.join(pkg, "launch", "drone_viz.rviz")

    exec_arg = DeclareLaunchArgument(
        "mission", # podemos utilizar outro valor que nao o padrao fazendo:
                    # ros2 launch itajuba_fase4 fase4.launch.py mission:='outro valor que nao o default'
        default_value="fase4",
        description="Drone following a lane on the ground"
    )

    lane_detector_node = Node(
        package='itajuba_cv_utils',
        executable='lane',
        parameters=[params]
    )

    fase_node = Node(
        package='itajuba_fase4',
        executable=LaunchConfiguration('mission'),
        parameters=[params],
        output='screen' # stdout e stderr vai para o terminal em que ros2 launch foi rodado
    )

    bridge_node = Node(
        package='itajuba_drone_lib',
        executable='pos_to_rviz', # um arquivo .cpp implementando um node de ros2
        output='screen'
    )

    delayed_fase_node = TimerAction(period=5.0, actions=[fase_node])

    return LaunchDescription([
        exec_arg,
        lane_detector_node,
        bridge_node,
        delayed_fase_node
    ])