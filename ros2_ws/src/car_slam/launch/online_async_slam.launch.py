import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    params_file = os.path.join(
        get_package_share_directory('car_slam'),
        'config',
        'slam_toolbox_params.yaml'
    )

    start_async_slam_toolbox_node = Node(
        parameters=[
            params_file,
            {'use_sim_time': False}
        ],
        package='slam_toolbox',
        executable='async_slam_toolbox_node',
        name='slam_toolbox',
        output='screen'
    )

    return LaunchDescription([
        start_async_slam_toolbox_node
    ])
