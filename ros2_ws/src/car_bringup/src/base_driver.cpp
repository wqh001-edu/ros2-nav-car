#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>

#pragma pack(push, 1)
struct CmdVelFrame {
    uint8_t header1 = 0x5A;
    uint8_t header2 = 0xA5;
    uint8_t type = 0x01;
    float target_vx;
    float target_wz;
    uint8_t checksum;
    uint8_t tail1 = 0x0D;
    uint8_t tail2 = 0x0A;
};

struct OdomFeedbackFrame {
    uint8_t header1;
    uint8_t header2;
    uint8_t type;
    float actual_vx;
    float actual_wz;
    float pos_x;
    float pos_y;
    float heading_yaw;
    uint8_t checksum;
    uint8_t tail1;
    uint8_t tail2;
};
#pragma pack(pop)

class BaseDriverNode : public rclcpp::Node {
public:
    BaseDriverNode() : Node("base_driver_node"), serial_fd_(-1) {
        this->declare_parameter<std::string>("port", "/dev/car_stm32");
        this->declare_parameter<int>("baudrate", 115200);

        std::string port = this->get_parameter("port").as_string();
        int baudrate = this->get_parameter("baudrate").as_int();

        RCLCPP_INFO(this->get_logger(), "Connecting to STM32 Base on %s at %d...", port.c_str(), baudrate);
        init_serial(port, baudrate);

        // 发布者与订阅者
        odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("odom", 50);
        tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
        cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "cmd_vel", 10, std::bind(&BaseDriverNode::cmd_vel_callback, this, std::placeholders::_1));

        // 50Hz 读取串口轮询定时器
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(20), std::bind(&BaseDriverNode::read_serial_loop, this));
    }

    ~BaseDriverNode() {
        if (serial_fd_ >= 0) close(serial_fd_);
    }

private:
    int serial_fd_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
    rclcpp::TimerBase::SharedPtr timer_;

    void init_serial(const std::string &port, int baudrate) {
        serial_fd_ = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (serial_fd_ < 0) {
            RCLCPP_WARN(this->get_logger(), "Failed to open serial port %s (Check udev rules or permissions)", port.c_str());
            return;
        }
        fcntl(serial_fd_, F_SETFL, 0);

        struct termios options;
        tcgetattr(serial_fd_, &options);
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_oflag &= ~OPOST;
        tcsetattr(serial_fd_, TCSANOW, &options);
    }

    void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        if (serial_fd_ < 0) return;

        CmdVelFrame frame;
        frame.target_vx = msg->linear.x;
        frame.target_wz = msg->angular.z;

        uint8_t sum = 0;
        uint8_t *p = reinterpret_cast<uint8_t*>(&frame);
        for (size_t i = 0; i < sizeof(CmdVelFrame) - 3; ++i) {
            sum += p[i];
        }
        frame.checksum = sum;

        write(serial_fd_, &frame, sizeof(CmdVelFrame));
    }

    void read_serial_loop() {
        if (serial_fd_ < 0) return;

        uint8_t buf[128];
        int n = read(serial_fd_, buf, sizeof(buf));
        if (n <= 0) return;

        // 模拟解析 OdomFeedbackFrame 数据
        for (int i = 0; i <= n - (int)sizeof(OdomFeedbackFrame); ++i) {
            if (buf[i] == 0x5A && buf[i+1] == 0xA5 && buf[i+2] == 0x02) {
                OdomFeedbackFrame *frame = reinterpret_cast<OdomFeedbackFrame*>(&buf[i]);
                if (frame->tail1 == 0x0D && frame->tail2 == 0x0A) {
                    publish_odom_and_tf(*frame);
                    break;
                }
            }
        }
    }

    void publish_odom_and_tf(const OdomFeedbackFrame &f) {
        auto now = this->now();

        tf2::Quaternion q;
        q.setRPY(0, 0, f.heading_yaw);

        // 发布 TF: odom -> base_footprint
        geometry_msgs::msg::TransformStamped t;
        t.header.stamp = now;
        t.header.frame_id = "odom";
        t.child_frame_id = "base_footprint";
        t.transform.translation.x = f.pos_x;
        t.transform.translation.y = f.pos_y;
        t.transform.translation.z = 0.0;
        t.transform.rotation.x = q.x();
        t.transform.rotation.y = q.y();
        t.transform.rotation.z = q.z();
        t.transform.rotation.w = q.w();
        tf_broadcaster_->sendTransform(t);

        // 发布 Odometry 消息
        nav_msgs::msg::Odometry odom;
        odom.header.stamp = now;
        odom.header.frame_id = "odom";
        odom.child_frame_id = "base_footprint";
        odom.pose.pose.position.x = f.pos_x;
        odom.pose.pose.position.y = f.pos_y;
        odom.pose.pose.orientation = t.transform.rotation;
        odom.twist.twist.linear.x = f.actual_vx;
        odom.twist.twist.angular.z = f.actual_wz;
        odom_pub_->publish(odom);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BaseDriverNode>());
    rclcpp::shutdown();
    return 0;
}
