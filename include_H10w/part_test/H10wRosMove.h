#pragma once

#include <rclcpp/rclcpp.hpp>
#include "controller/msg/move_message.hpp"
#include "controller/msg/error_message.hpp"
#include "controller/srv/multi_joint_move.hpp"
#include "controller/srv/single_joint_move.hpp"
#include "controller/srv/linear_move.hpp"
#include "controller/srv/enable_realtime_cmd.hpp"
#include "controller/msg/real_time_body_joints.hpp"
#include "controller/msg/real_time_body_tcp_cartesian.hpp"
#include "DeviceControlServiceClient.h"
#include "humanoid_controller_client.h"
#include "H10Wglobalconstants.h"

using namespace GlobalConstants::H10W;

class H10wRosMove : public rclcpp::Node
{
public:
    explicit H10wRosMove(const std::string &strIpPort = "localhost");
    ~H10wRosMove() = default;

    void stopTest();
    bool has_move_msg() const { return get_move_msg_ != nullptr; }

private:
    void move_callback(const controller::msg::MoveMessage::SharedPtr msg);
    void error_callback(const controller::msg::ErrorMessage::SharedPtr msg);

public:
    void ros_singlemove(const uint32_t joint_index, const double target_position, const double velocity);
    void ros_multimove(const std::vector<int32_t> &joint_indices, const std::vector<float> &target_positions, const std::vector<float> &velocities);
    void ros_linearmove(int32_t count, const std::vector<int32_t> &type, std::vector<std::vector<double>> &pose, const std::vector<float> velocity_percent, std::vector<float> acceleration_percent);
    void enable_realtime_cmd(bool m_enable);
    void servoj(controller::msg::RealTimeBodyJoints msg,double m_step);
    void servol(controller::msg::RealTimeBodyTcpCartesian msg,double m_step);
    void speedj(controller::msg::RealTimeBodyJoints msg,int32_t t);
    void speedl(controller::msg::RealTimeBodyTcpCartesian msg,int32_t t);

public:
    std::shared_ptr<grpc::Channel> m_channel;
    std::unique_ptr<CDeviceControlServiceClient> m_pDevCtrlSvrClient;
    std::unique_ptr<HumanoidControllerClient> m_pControllerClient;
    controller::msg::MoveMessage::SharedPtr get_move_msg_ = nullptr;
    controller::msg::ErrorMessage::SharedPtr get_error_msg_ = nullptr;

protected:
    rclcpp::Subscription<controller::msg::MoveMessage>::SharedPtr
        move_subscriber_;
    rclcpp::Subscription<controller::msg::ErrorMessage>::SharedPtr
        error_subscriber_;
    rclcpp::Client<controller::srv::MultiJointMove>::SharedPtr
        m_multi_move_client;
    rclcpp::Client<controller::srv::SingleJointMove>::SharedPtr
        m_single_move_client;
    rclcpp::Client<controller::srv::LinearMove>::SharedPtr
        m_linear_move_client;
    rclcpp::Client<controller::srv::EnableRealtimeCmd>::SharedPtr
        m_enable_realtime_cmd;
    rclcpp::Publisher<controller::msg::RealTimeBodyJoints>::SharedPtr
        m_servo_body_j_publisher;
    rclcpp::Publisher<controller::msg::RealTimeBodyTcpCartesian>::SharedPtr
        m_servo_body_l_publisher;
    rclcpp::Publisher<controller::msg::RealTimeBodyJoints>::SharedPtr
        m_speed_body_j_publisher;
    rclcpp::Publisher<controller::msg::RealTimeBodyTcpCartesian>::SharedPtr
        m_speed_body_l_publisher;
    std::mutex msg_mutex_;
    std::atomic<bool> move_done_{false};
};