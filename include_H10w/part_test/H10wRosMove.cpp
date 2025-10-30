#include <cassert>
#include <cmath>
#include <iostream>
#include <array>
#include <chrono>
#include <thread>

#include "H10wRosMove.h"

H10wRosMove::H10wRosMove(const std::string &strIpPort)
    : Node("H10wRosMove"),
      m_pDevCtrlSvrClient(std::make_unique<CDeviceControlServiceClient>(
          grpc::CreateChannel(strIpPort + ":8686", grpc::InsecureChannelCredentials()))),
      m_pControllerClient(std::make_unique<HumanoidControllerClient>(
          grpc::CreateChannel(strIpPort + ":8585", grpc::InsecureChannelCredentials())))
          
{
    rclcpp::QoS qos =
        rclcpp::QoS(rclcpp::KeepLast(1))
            .reliability(rclcpp::ReliabilityPolicy::BestEffort)
            .durability(rclcpp::DurabilityPolicy::Volatile)
            .history(rclcpp::HistoryPolicy::KeepLast);

    move_subscriber_ = this->create_subscription<controller::msg::MoveMessage>(
        "move_message", qos,
        std::bind(&H10wRosMove::move_callback, this,
                  std::placeholders::_1));

    error_subscriber_ = this->create_subscription<controller::msg::ErrorMessage>(
        "error_message", qos,
        std::bind(&H10wRosMove::error_callback, this,
                  std::placeholders::_1));
    m_enable_realtime_cmd =
        this->create_client<controller::srv::EnableRealtimeCmd>(
            "/h10w/controller/enable_realtime_cmd");

    RCLCPP_INFO(this->get_logger(), "MoveSubscriber 节点已启动，等待消息...");
    m_single_move_client =
        this->create_client<controller::srv::SingleJointMove>(
            "/h10w/controller/single_move");
    m_multi_move_client = this->create_client<controller::srv::MultiJointMove>(
        "/h10w/controller/multi_move");
    m_linear_move_client = this->create_client<controller::srv::LinearMove>(
        "/h10w/controller/linear_move");
    m_servo_body_j_publisher = this->create_publisher<controller::msg::RealTimeBodyJoints>(
        "controller/servo_body_j", qos);
    m_servo_body_l_publisher =
        this->create_publisher<controller::msg::RealTimeBodyTcpCartesian>(
            "controller/servo_body_l", qos);
    m_speed_body_j_publisher =
        this->create_publisher<controller::msg::RealTimeBodyJoints>(
            "controller/speed_body_j", qos);
    m_speed_body_l_publisher =
        this->create_publisher<controller::msg::RealTimeBodyTcpCartesian>(
            "controller/speed_body_l", qos);
    RCLCPP_INFO(this->get_logger(), "client 节点已启动，等待匹配...");
}
void H10wRosMove::move_callback(const controller::msg::MoveMessage::SharedPtr msg)
{
    std::lock_guard<std::mutex> lock(msg_mutex_);
    get_move_msg_ = msg;
    // std::string position_str = "[";
    // std::string velocity_str = "[";
    // for (size_t i = 0; i < msg->position.size(); ++i)
    // {
    //     position_str += std::to_string(msg->position[i]);
    //     velocity_str += std::to_string(msg->velocity[i]);
    //     if (i < msg->position.size() - 1)
    //     {
    //         position_str += ", ";
    //         velocity_str += ", ";
    //     }
    // }
    // position_str += "]";
    // velocity_str += "]";

    // RCLCPP_INFO(this->get_logger(),
    //             "接收到 MoveMessage: 状态=%d, 位置=%s, 速度=%s,token=%d", msg->state,
    //             position_str.c_str(), velocity_str.c_str(), msg->token);
}
void H10wRosMove::error_callback(const controller::msg::ErrorMessage::SharedPtr msg)
{
    std::lock_guard<std::mutex> lock(msg_mutex_);
    get_error_msg_ = msg;
    // if (msg->error_code != 0)
    // {
    //     RCLCPP_ERROR(
    //         this->get_logger(),
    //         "接收到 ErrorMessage: ID=%u, 级别=%u, 模块=%u, 错误码=%u, 消息='%s'",
    //         msg->id, msg->level, msg->module, msg->error_code, msg->msg.c_str());
    // }
}

void H10wRosMove::enable_realtime_cmd(bool m_enable)
{
    /*等待服务端上线*/
    while (!m_enable_realtime_cmd->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request =
        std::make_shared<controller::srv::EnableRealtimeCmd::Request>();
    request->enable = m_enable;
    // 发送异步请求，然后等待返回，返回时调用回调函数
    auto result_future = m_enable_realtime_cmd->async_send_request(request);
    try
    {
        auto response = result_future.get(); // 阻塞直到结果返回
        RCLCPP_INFO(this->get_logger(), "收到实时指令使能结果：%d", response->success);
    }
    catch (const std::exception &e)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败: %s", e.what());
    }
}

void H10wRosMove::stopTest()
{
    m_pControllerClient->stop();
    m_pDevCtrlSvrClient->controlBrakeStatus(BRAKE_STATUS::OFF, true);
    // m_pDevCtrlSvrClient->controlPowerStatus(POWER_STATUS::OFF);
    rclcpp::shutdown();
}

void H10wRosMove::ros_singlemove(const uint32_t joint_index, const double target_position, const double velocity)
{
    /*等待服务端上线*/
    while (!m_single_move_client->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }
    // 构造请求
    auto request =
        std::make_shared<controller::srv::SingleJointMove::Request>();
    request->joint_index = joint_index;
    request->target_position = target_position;
    request->velocity = velocity;

    auto result_future = m_single_move_client->async_send_request(request);
    try
    {
        // 等待服务响应
        auto response = result_future.get();
        RCLCPP_INFO(this->get_logger(), "收到移动结果：%d", response->success);

        // 设置超时时间（例如5秒，可根据实际情况调整）
        const int timeout_ms = 5000;
        const int check_interval_ms = 10;
        int elapsed_ms = 0;

        while (true)
        {
            // 检查是否达到目标位置
            if (isFloatValueEqual(get_move_msg_->position[joint_index - 1], target_position, 0.001) && get_move_msg_->state == 0)
            {
                std::cout << "运动完成" << "\n";
                break;
            }

            // error
            if (get_error_msg_->error_code != 0)
            {
                RCLCPP_ERROR(this->get_logger(),
                             "message:module=%u, error_code=%d, msg=%s",
                             get_error_msg_->module,
                             get_error_msg_->error_code,
                             get_error_msg_->msg.c_str());
                return;
            }

            // 检查是否超时
            if (elapsed_ms >= timeout_ms)
            {
                RCLCPP_ERROR(this->get_logger(), "运动超时！目标位置: %.3f, 当前位置: %.3f",
                             target_position, get_move_msg_->position[joint_index - 1]);
                // 可以根据需要添加超时处理逻辑，如强制停止等
                return; // 超时退出
            }

            // 休眠一小段时间再检查
            sleepMilliseconds(check_interval_ms);
            elapsed_ms += check_interval_ms;

            // 检查系统状态是否正常
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(this->get_logger(), "系统状态异常，退出运动检查");
                return;
            }
        }
    }
    catch (const std::exception &e)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败: %s", e.what());
    }
}

void H10wRosMove::ros_multimove(const std::vector<int32_t> &joint_indices, const std::vector<float> &target_positions, const std::vector<float> &velocities)
{
    /*等待服务端上线*/
    while (!m_multi_move_client->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 检查输入参数有效性
    if (joint_indices.size() != target_positions.size() ||
        joint_indices.size() != velocities.size())
    {
        RCLCPP_ERROR(this->get_logger(), "关节参数向量长度不匹配");
        return;
    }
    if (joint_indices.empty())
    {
        RCLCPP_WARN(this->get_logger(), "未提供任何关节运动参数");
        return;
    }

    std::vector<controller::msg::JointAngle> joint_angles;
    // 遍历参数构造关节角度信息
    for (size_t i = 0; i < joint_indices.size(); ++i)
    {
        controller::msg::JointAngle joint_angle;
        joint_angle.joint_index = joint_indices[i];
        joint_angle.target_position = target_positions[i];
        joint_angle.velocity = velocities[i];
        joint_angles.emplace_back(joint_angle);
    }

    // 构造请求
    auto request = std::make_shared<controller::srv::MultiJointMove::Request>();
    request->joint_angles = joint_angles;

    auto result_future = m_multi_move_client->async_send_request(request);
    try
    {
        auto response = result_future.get(); // 阻塞直到结果返回
        RCLCPP_INFO(this->get_logger(), "收到移动结果：%d", response->success);

        // 设置超时时间（例如8秒，多关节运动可适当延长）
        const int timeout_ms = 8000;
        const int check_interval_ms = 10;
        int elapsed_ms = 0;

        while (true)
        {
            bool all_joints_reached = true;

            // 检查所有关节是否都达到目标位置且状态正常
            for (size_t i = 0; i < joint_indices.size(); ++i)
            {
                int32_t joint_idx = joint_indices[i];
                double target_pos = target_positions[i];

                // 检查单个关节是否到位
                if (!isFloatValueEqual(get_move_msg_->position[joint_idx - 1], target_pos, 0.001) || get_move_msg_->state != 0)
                {
                    all_joints_reached = false;
                    break; // 只要有一个关节未到位，就跳出检查循环
                }
            }

            // 如果所有关节都到位，退出等待循环
            if (all_joints_reached)
            {
                std::cout << "所有关节运动完成" << "\n";
                break;
            }

            // error
            if (get_error_msg_->error_code != 0)
            {
                RCLCPP_ERROR(this->get_logger(),
                             "message:module=%u, error_code=%d, msg=%s",
                             get_error_msg_->module,
                             get_error_msg_->error_code,
                             get_error_msg_->msg.c_str());
                return;
            }

            // 检查是否超时
            if (elapsed_ms >= timeout_ms)
            {
                RCLCPP_ERROR(this->get_logger(), "运动超时！部分关节未达到目标位置");
                // 输出未到位的关节信息，便于调试
                for (size_t i = 0; i < joint_indices.size(); ++i)
                {
                    int32_t joint_idx = joint_indices[i];
                    double target_pos = target_positions[i];
                    if (!isFloatValueEqual(get_move_msg_->position[joint_idx - 1], target_pos, 0.001))
                    {
                        RCLCPP_ERROR(this->get_logger(), "关节 %d: 目标位置 %.3f, 当前位置 %.3f",
                                     joint_idx, target_pos, get_move_msg_->position[joint_idx - 1]);
                    }
                }
                return; // 超时退出
            }

            // 休眠一小段时间再检查
            sleepMilliseconds(check_interval_ms);
            elapsed_ms += check_interval_ms;

            // 检查系统状态是否正常
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(this->get_logger(), "系统状态异常，退出运动检查");
                return;
            }
        }
    }
    catch (const std::exception &e)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败: %s", e.what());
    }
}

void H10wRosMove::ros_linearmove(int32_t count, const std::vector<int32_t> &type, std::vector<std::vector<double>> &pose, const std::vector<float> velocity_percent, std::vector<float> acceleration_percent)
{
    while (!m_linear_move_client->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    std::vector<controller::msg::LinearMoveParams> linear_params;
    for (size_t i = 0; i < count; ++i)
    {
        controller::msg::LinearMoveParams linear_param;
        linear_param.type = type[i];
        linear_param.pose = pose[i];
        linear_param.velocity_percent = velocity_percent[i];
        linear_param.acceleration_percent = acceleration_percent[i];
        linear_params.emplace_back(linear_param);
    }
    // 构造请求
    auto request =
        std::make_shared<controller::srv::LinearMove::Request>();
    request->linear_move = linear_params;
    auto result_future = m_linear_move_client->async_send_request(request);
    try
    {
        // 等待服务响应
        auto response = result_future.get();
        // RCLCPP_INFO(this->get_logger(), "收到移动结果：%d", response->success);

        // 设置超时时间（例如5秒，可根据实际情况调整）
        const int timeout_ms = 5000;
        const int check_interval_ms = 10;
        int elapsed_ms = 0;

        while (true)
        {
            bool all_reached = true;
            // 检查是否满足完成条件
            for (int i = 0; i < count; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    if (!isFloatValueEqual(get_move_msg_->tcp_pose[type[i] - 1].pose[j], pose[i][j], 0.001))
                    {
                        all_reached = false;
                        break;
                    }
                }
            }
            if (all_reached && get_move_msg_->state == 0)
            {
                RCLCPP_INFO(this->get_logger(),
                            "grpc_linearmove() 完成");
                return;
            }

            // error
            if (get_error_msg_->error_code != 0)
            {
                RCLCPP_ERROR(this->get_logger(),
                             "message:module=%u, error_code=%d, msg=%s",
                             get_error_msg_->module,
                             get_error_msg_->error_code,
                             get_error_msg_->msg.c_str());
                return;
            }

            // 检查是否超时
            if (elapsed_ms >= timeout_ms)
            {
                RCLCPP_ERROR(this->get_logger(), "运动超时！");
                // 可以根据需要添加超时处理逻辑，如强制停止等
                return; // 超时退出
            }

            // 休眠一小段时间再检查
            sleepMilliseconds(check_interval_ms);
            elapsed_ms += check_interval_ms;

            // 检查系统状态是否正常
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(this->get_logger(), "系统状态异常，退出运动检查");
                return;
            }
        }
    }
    catch (const std::exception &e)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败: %s", e.what());
    }
}

void H10wRosMove::servoj(controller::msg::RealTimeBodyJoints msg, double m_step)
{
    if (!has_move_msg())
        return;

    move_done_ = false; // 重置状态
    auto message = controller::msg::RealTimeBodyJoints();
    message.left_arm_valid = msg.left_arm_valid;
    message.right_arm_valid = msg.right_arm_valid;
    message.torso_valid = msg.torso_valid;
    message.time = msg.time;

    // 复制当前位置
    for (int i = 0; i < 7; ++i)
    {
        message.left_arm[i] = get_move_msg_->position[i];
        message.right_arm[i] = get_move_msg_->position[i + 7];
    }
    for (int i = 0; i < 3; ++i)
    {
        message.torso[i] = get_move_msg_->position[i + 14];
    }

    while (rclcpp::ok() && !move_done_.load())
    {
        bool all_reached = true;

        // 左臂
        if (message.left_arm_valid)
        {
            for (int i = 0; i < 7; ++i)
            {
                double &cur = message.left_arm[i];
                double tar = msg.left_arm[i];
                double delta = tar - cur;
                if (std::fabs(delta) > m_step)
                {
                    double inc = (delta > 0 ? 1.0 : -1.0) * m_step;
                    cur += inc;
                    all_reached = false;
                }
                else
                {
                    cur = tar;
                }
                message.left_arm[i] = cur;
            }
        }

        // 右臂
        if (message.right_arm_valid)
        {
            for (int i = 0; i < 7; ++i)
            {
                double &cur = message.right_arm[i];
                double tar = msg.right_arm[i];
                double delta = tar - cur;
                if (std::fabs(delta) > m_step)
                {
                    double inc = (delta > 0 ? 1.0 : -1.0) * m_step;
                    cur += inc;
                    all_reached = false;
                }
                else
                {
                    cur = tar;
                }
                message.right_arm[i] = cur;
            }
        }

        // torso
        if (message.torso_valid)
        {
            for (int i = 0; i < 3; ++i)
            {
                double &cur = message.torso[i];
                double tar = msg.torso[i];
                double delta = tar - cur;
                if (std::fabs(delta) > m_step)
                {
                    double inc = (delta > 0 ? 1.0 : -1.0) * m_step;
                    cur += inc;
                    all_reached = false;
                }
                else
                {
                    cur = tar;
                }
                message.torso[i] = cur;
            }
        }

        m_servo_body_j_publisher->publish(message);

        // 检查是否到达目标
        if (all_reached)
        {
            move_done_ = true;
            RCLCPP_INFO(this->get_logger(), "servoj reached target, exit loop");
        }

        // error
        if (get_error_msg_->error_code != 0)
        {
            RCLCPP_ERROR(this->get_logger(),
                         "message:module=%u, error_code=%d, msg=%s",
                         get_error_msg_->module,
                         get_error_msg_->error_code,
                         get_error_msg_->msg.c_str());
            return;
        }

        std::this_thread::sleep_for(std::chrono::duration<double>(message.time));
    }
}

void H10wRosMove::servol(controller::msg::RealTimeBodyTcpCartesian msg, double m_step)
{
    if (!has_move_msg())
        return;
    move_done_ = false;
    auto message = controller::msg::RealTimeBodyTcpCartesian();
    message.left_arm_valid = msg.left_arm_valid;
    message.right_arm_valid = msg.right_arm_valid;
    message.torso_valid = msg.torso_valid;
    message.time = msg.time;

    for (int i = 0; i < 6; i++)
    {
        message.left_arm[i] = get_move_msg_->tcp_pose[0].pose[i];
        message.right_arm[i] = get_move_msg_->tcp_pose[1].pose[i];
        message.torso[i] = get_move_msg_->tcp_pose[2].pose[i];
    }

    while (rclcpp::ok() && !move_done_.load())
    {
        bool all_reached = true;
        // 左臂
        if (message.left_arm_valid)
        {
            for (int i = 0; i < 6; ++i)
            {
                double &cur = message.left_arm[i];
                double tar = msg.left_arm[i];
                double delta = tar - cur;
                if (std::fabs(delta) > m_step)
                { // 未到目标
                    double inc = (delta > 0 ? 1.0 : -1.0) * m_step;
                    cur += inc;
                    all_reached = false;
                }
                else
                {
                    cur = tar;
                }
                message.left_arm[i] = cur;
            }
        }

        // 右臂
        if (message.right_arm_valid)
        {
            for (int i = 0; i < 6; ++i)
            {
                double &cur = message.right_arm[i];
                double tar = msg.right_arm[i];
                double delta = tar - cur;
                if (std::fabs(delta) > m_step)
                { // 未到目标
                    double inc = (delta > 0 ? 1.0 : -1.0) * m_step;
                    cur += inc;
                    all_reached = false;
                }
                else
                {
                    cur = tar;
                }
                message.right_arm[i] = cur;
            }
        }

        // 躯干
        if (message.torso_valid)
        {
            for (int i = 0; i < 6; ++i)
            {
                double &cur = message.torso[i];
                double tar = msg.torso[i];
                double delta = tar - cur;
                if (std::fabs(delta) > m_step)
                { // 未到目标
                    double inc = (delta > 0 ? 1.0 : -1.0) * m_step;
                    cur += inc;
                    all_reached = false;
                }
                else
                {
                    cur = tar;
                }
                message.torso[i] = cur;
            }
        }
        m_servo_body_l_publisher->publish(message);

        if (all_reached)
        {
            move_done_ = true;
            RCLCPP_INFO(this->get_logger(), "servol reached target, exit loop");
        }

        // error
        if (get_error_msg_->error_code != 0)
        {
            RCLCPP_ERROR(this->get_logger(),
                         "message:module=%u, error_code=%d, msg=%s",
                         get_error_msg_->module,
                         get_error_msg_->error_code,
                         get_error_msg_->msg.c_str());
            return;
        }
        std::this_thread::sleep_for(std::chrono::duration<double>(message.time));
    }
}

void H10wRosMove::speedj(controller::msg::RealTimeBodyJoints msg, int32_t t)
{
    move_done_ = false;
    auto message = controller::msg::RealTimeBodyJoints();
    message.left_arm_valid = msg.left_arm_valid;
    message.right_arm_valid = msg.right_arm_valid;
    message.torso_valid = msg.torso_valid;
    message.time = msg.time;
    int a = 0;
    while (rclcpp::ok() && !move_done_.load())
    {
        for (int i = 0; i < 7; i++)
        {
            message.left_arm[i] = msg.left_arm[i];
            message.right_arm[i] = msg.right_arm[i];
        }
        for (int i = 0; i < 3; i++)
        {
            message.torso[i] = msg.torso[i];
        }

        m_speed_body_j_publisher->publish(message);
        std::this_thread::sleep_for(std::chrono::duration<double>(message.time));
        a++;
        if (a == t)
        {
            for (int i = 0; i < 7; i++)
            {
                message.left_arm[i] = 0;
                message.right_arm[i] = 0;
            }
            for (int i = 0; i < 3; i++)
            {
                message.torso[i] = 0;
            }
            m_speed_body_j_publisher->publish(message);
            move_done_ = true;
            RCLCPP_INFO(this->get_logger(), "speedj reached target, exit loop");
        }
        // error
        if (get_error_msg_->error_code != 0)
        {
            RCLCPP_ERROR(this->get_logger(),
                         "message:module=%u, error_code=%d, msg=%s",
                         get_error_msg_->module,
                         get_error_msg_->error_code,
                         get_error_msg_->msg.c_str());
            return;
        }
    }
}

void H10wRosMove::speedl(controller::msg::RealTimeBodyTcpCartesian msg, int32_t t)
{
    move_done_ = false;
    auto message = controller::msg::RealTimeBodyTcpCartesian();

    message.left_arm_valid = msg.left_arm_valid;
    message.right_arm_valid = msg.right_arm_valid;
    message.torso_valid = msg.torso_valid;
    message.time = msg.time;
    int a = 0;

    while (rclcpp::ok() && !move_done_.load())
    {
        for (int i = 0; i < 6; i++)
        {
            message.left_arm[i] = msg.left_arm[i];
            message.right_arm[i] = msg.right_arm[i];
            message.torso[i] = msg.torso[i];
        }
        m_speed_body_l_publisher->publish(message);
        std::this_thread::sleep_for(std::chrono::duration<double>(message.time));
        a++;
        if (a == t)
        {
            for (int i = 0; i < 6; i++)
            {
                message.left_arm[i] = 0;
                message.right_arm[i] = 0;
                message.torso[i] = 0;
            }
            m_speed_body_l_publisher->publish(message);
            move_done_ = true;
            RCLCPP_INFO(this->get_logger(), "speedl reached target, exit loop");
        }
        // error
        if (get_error_msg_->error_code != 0)
        {
            RCLCPP_ERROR(this->get_logger(),
                         "message:module=%u, error_code=%d, msg=%s",
                         get_error_msg_->module,
                         get_error_msg_->error_code,
                         get_error_msg_->msg.c_str());
            return;
        }
    }
}