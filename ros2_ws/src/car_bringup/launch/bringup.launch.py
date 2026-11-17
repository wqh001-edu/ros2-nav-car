from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    # 1. STM32 底盘串口驱动节点
    base_driver_node = Node(
        package='car_bringup',
        executable='base_driver',
        name='base_driver',
        output='screen',
        parameters=[{
            'port': '/dev/car_stm32',
            'baudrate': 115200
        }]
    )

    # 2. 静态 TF 发布 (base_footprint -> base_link -> laser_link)
    base_to_laser_tf = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        arguments=['0.10', '0.0', '0.15', '0.0', '0.0', '0.0', 'base_link', 'laser_link'],
        output='screen'
    )
    
    footprint_to_base_tf = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        arguments=['0.0', '0.0', '0.05', '0.0', '0.0', '0.0', 'base_footprint', 'base_link'],
        output='screen'
    )

    return LaunchDescription([
        base_driver_node,
        footprint_to_base_tf,
        base_to_laser_tf
    ])
