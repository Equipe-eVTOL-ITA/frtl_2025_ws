import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    pkg_fase4 = get_package_share_directory('itajuba_fase4')
    rviz_cfg_path = os.path.join(pkg_fase4, 'launch', 'drone_viz.rviz')

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_cfg_path],
        output='screen'
    )

    return LaunchDescription([
        rviz_node
    ])