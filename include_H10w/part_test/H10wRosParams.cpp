#include <cassert>
#include <cmath>
#include <iostream>
#include <array>
#include <chrono>
#include <thread>

#include "H10wRosParams.h"

H10wRosParams::H10wRosParams(const std::string &strIpPort)
    : Node("H10wRosParams")
        
{
    // RCLCPP_INFO(this->get_logger(), "MoveSubscriber 节点已启动，等待消息...");

    m_get_soft_limit_client =
        this->create_client<controller::srv::GetJointSoftLimit>(
            "/h10w//controller/get_joint_soft_limit");
    m_get_joint_max_vel_client =
        this->create_client<controller::srv::GetJointMaxVel>(
            "/h10w/controller/get_joint_max_vel");
    m_get_joint_max_acc_client =
        this->create_client<controller::srv::GetJointMaxAcc>(
            "/h10w/controller/get_joint_max_acc");
    m_get_cart_trans_max_vel_client =
        this->create_client<controller::srv::GetCartesianTranslationMaxVel>(
            "/h10w/controller/get_cart_trans_max_vel");
    m_get_cart_trans_max_acc_client =
        this->create_client<controller::srv::GetCartesianTranslationMaxAcc>(
            "/h10w/controller/get_cart_trans_max_acc");
    m_get_cart_rota_max_vel_client =
        this->create_client<controller::srv::GetCartesianRotationMaxVel>(
            "/h10w/controller/get_cart_rota_max_vel");
    m_get_cart_rota_max_acc_client =
        this->create_client<controller::srv::GetCartesianRotationMaxAcc>(
            "/h10w/controller/get_cart_rota_max_acc");
    m_set_soft_limit_client =
        this->create_client<controller::srv::SetJointSoftLimit>(
            "/h10w/controller/set_joint_soft_limit");
    m_set_joint_max_acc_client =
        this->create_client<controller::srv::SetJointMaxAcc>(
            "/h10w/controller/set_joint_max_acc");
    m_set_joint_max_vel_client =
        this->create_client<controller::srv::SetJointMaxVel>(
            "/h10w/controller/set_joint_max_vel");
    m_set_cart_trans_max_vel_client =
        this->create_client<controller::srv::SetCartesianTranslationMaxVel>(
            "/h10w/controller/set_cart_trans_max_vel");
    m_set_cart_trans_max_acc_client =
        this->create_client<controller::srv::SetCartesianTranslationMaxAcc>(
            "/h10w/controller/set_cart_trans_max_acc");
    m_set_cart_rota_max_vel_client =
        this->create_client<controller::srv::SetCartesianRotationMaxVel>(
            "/h10w/controller/set_cart_rota_max_vel");
    m_set_cart_rota_max_acc_client =
        this->create_client<controller::srv::SetCartesianRotationMaxAcc>(
            "/h10w/controller/set_cart_rota_max_acc");
    m_get_mech_limit_client =
        this->create_client<controller::srv::GetJointMechanicalLimit>(
            "/h10w/controller/get_joint_mech_limit");
    m_get_mech_max_vel_client =
        this->create_client<controller::srv::GetJointMechanicalMaxVel>(
            "/h10w/controller/get_joint_mech_max_vel");
    m_get_mech_max_acc_client =
        this->create_client<controller::srv::GetJointMechanicalMaxAcc>(
            "/h10w/controller/get_joint_mech_max_acc");
    m_get_cart_mech_trans_max_vel_client = this->create_client<
        controller::srv::GetCartesianMechanicalTranslationMaxVel>(
        "/h10w/controller/get_cart_mech_trans_max_vel");
    m_get_cart_mech_trans_max_acc_client = this->create_client<
        controller::srv::GetCartesianMechanicalTranslationMaxAcc>(
        "/h10w/controller/get_cart_mech_trans_max_acc");
    m_get_cart_mech_rota_max_vel_client = this->create_client<
        controller::srv::GetCartesianMechanicalRotationMaxVel>(
        "/h10w/controller/get_cart_mech_rota_max_vel");
    m_get_cart_mech_rota_max_acc_client = this->create_client<
        controller::srv::GetCartesianMechanicalRotationMaxAcc>(
        "/h10w/controller/get_cart_mech_rota_max_acc");
    RCLCPP_INFO(this->get_logger(), "client 节点已启动，等待匹配...");
}

bool H10wRosParams::get_joint_soft_limit(std::vector<controller::msg::JointParams> &joint_params)
{
    /*等待服务端上线*/
    while (!m_get_soft_limit_client->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    auto request =
        std::make_shared<controller::srv::GetJointSoftLimit::Request>();
    // 发送异步请求，然后等待返回
    auto result_future = m_get_soft_limit_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_soft_limit_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    joint_params = result->joint_params;
    return true;
}

bool H10wRosParams::get_joint_max_vel(std::vector<controller::msg::JointParams> &joint_params)
{
    /*等待服务端上线*/
    while (!m_get_joint_max_vel_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    auto request = std::make_shared<controller::srv::GetJointMaxVel::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_joint_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_joint_max_vel_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    joint_params = result->joint_params;
    return true;
}

bool H10wRosParams::get_joint_max_acc(std::vector<controller::msg::JointParams> &joint_params)
{
    /*等待服务端上线*/
    while (!m_get_joint_max_acc_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    auto request = std::make_shared<controller::srv::GetJointMaxAcc::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_joint_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_joint_max_acc_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    joint_params = result->joint_params;
    return true;
}

bool H10wRosParams::get_cart_trans_max_vel(std::vector<controller::msg::CartesianParams> &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_get_cart_trans_max_vel_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::GetCartesianTranslationMaxVel::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_cart_trans_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_cart_trans_max_vel_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    cartesian_params = result->cartesian_params;
    return true;
}

bool H10wRosParams::get_cart_trans_max_acc(std::vector<controller::msg::CartesianParams> &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_get_cart_trans_max_acc_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::GetCartesianTranslationMaxAcc::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_cart_trans_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_cart_trans_max_acc_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    cartesian_params = result->cartesian_params;
    return true;
}
bool H10wRosParams::get_cart_rota_max_vel(std::vector<controller::msg::CartesianParams> &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_get_cart_rota_max_vel_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::GetCartesianRotationMaxVel::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_cart_rota_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_cart_rota_max_vel_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    cartesian_params = result->cartesian_params;
    return true;
}

bool H10wRosParams::get_cart_rota_max_acc(std::vector<controller::msg::CartesianParams> &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_get_cart_rota_max_acc_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::GetCartesianRotationMaxAcc::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_cart_rota_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_cart_rota_max_acc_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    cartesian_params = result->cartesian_params;
    return true;
}

bool H10wRosParams::set_joint_soft_limit(const JointLimit &joint_params)
{
    /*等待服务端上线*/
    while (
        !m_set_soft_limit_client->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request =
        std::make_shared<controller::srv::SetJointSoftLimit::Request>();
    request->joint_params.clear();
    for (const auto &param : joint_params)
    {
        controller::msg::JointParams ddsParam;
        ddsParam.joint_index = std::get<0>(param);
        ddsParam.max_pos = std::get<1>(param);
        ddsParam.min_pos = std::get<2>(param);
        request->joint_params.push_back(ddsParam);
    }
    // 发送异步请求，然后等待返回
    auto result_future = m_set_soft_limit_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_set_soft_limit_client->remove_pending_request(result_future);
        return false;
    }
    auto response = result_future.get();
    return response->success;
}

bool H10wRosParams::set_joint_max_vel(const JointMaxVel &joint_params)
{
    /*等待服务端上线*/
    while (!m_set_joint_max_vel_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<controller::srv::SetJointMaxVel::Request>();
    request->joint_params.clear();
    for (const auto &param : joint_params)
    {
        controller::msg::JointParams ddsParam;
        ddsParam.joint_index = std::get<0>(param);
        ddsParam.max_vel = std::get<1>(param);
        request->joint_params.push_back(ddsParam);
    }
    // 发送异步请求，然后等待返回
    auto result_future =
        m_set_joint_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_set_joint_max_vel_client->remove_pending_request(result_future);
        return false;
    }
    auto response = result_future.get();
    return response->success;
}

bool H10wRosParams::set_joint_max_acc(const JointMaxAcc &joint_params)
{
    /*等待服务端上线*/
    while (!m_set_joint_max_acc_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<controller::srv::SetJointMaxAcc::Request>();
    request->joint_params.clear();
    for (const auto &param : joint_params)
    {
        controller::msg::JointParams ddsParam;
        ddsParam.joint_index = std::get<0>(param);
        ddsParam.max_acc = std::get<1>(param);
        request->joint_params.push_back(ddsParam);
    }
    // 发送异步请求，然后等待返回
    auto result_future =
        m_set_joint_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_set_joint_max_acc_client->remove_pending_request(result_future);
        return false;
    }
    auto response = result_future.get();
    return response->success;
}

bool H10wRosParams::set_cart_trans_max_vel(const CartTransVel &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_set_cart_trans_max_vel_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::SetCartesianTranslationMaxVel::Request>();
    request->cartesian_params.clear();
    for (const auto &param : cartesian_params)
    {
        controller::msg::CartesianParams ddsParam;
        ddsParam.cartesian_index = std::get<0>(param);
        ddsParam.trans_max_vel = std::get<1>(param);
        request->cartesian_params.push_back(ddsParam);
    }
    // 发送异步请求，然后等待返回
    auto result_future =
        m_set_cart_trans_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_set_cart_trans_max_vel_client->remove_pending_request(result_future);
        return false;
    }
    auto response = result_future.get();
    return response->success;
}

bool H10wRosParams::set_cart_trans_max_acc(const CartTransAcc &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_set_cart_trans_max_acc_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::SetCartesianTranslationMaxAcc::Request>();
    request->cartesian_params.clear();
    for (const auto &param : cartesian_params)
    {
        controller::msg::CartesianParams ddsParam;
        ddsParam.cartesian_index = std::get<0>(param);
        ddsParam.trans_max_acc = std::get<1>(param);
        request->cartesian_params.push_back(ddsParam);
    }
    // 发送异步请求，然后等待返回
    auto result_future =
        m_set_cart_trans_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_set_cart_trans_max_acc_client->remove_pending_request(result_future);
        return false;
    }
    auto response = result_future.get();
    return response->success;
}

bool H10wRosParams::set_cart_rota_max_vel(const CartRotaVel &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_set_cart_rota_max_vel_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::SetCartesianRotationMaxVel::Request>();
    request->cartesian_params.clear();
    for (const auto &param : cartesian_params)
    {
        controller::msg::CartesianParams ddsParam;
        ddsParam.cartesian_index = std::get<0>(param);
        ddsParam.rota_max_vel = std::get<1>(param);
        request->cartesian_params.push_back(ddsParam);
    }
    // 发送异步请求，然后等待返回
    auto result_future =
        m_set_cart_rota_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_set_cart_rota_max_vel_client->remove_pending_request(result_future);
        return false;
    }
    auto response = result_future.get();
    return response->success;
}

bool H10wRosParams::set_cart_rota_max_acc(const CartRotaAcc &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_set_cart_rota_max_acc_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::SetCartesianRotationMaxAcc::Request>();
    request->cartesian_params.clear();
    for (const auto &param : cartesian_params)
    {
        controller::msg::CartesianParams ddsParam;
        ddsParam.cartesian_index = std::get<0>(param);
        ddsParam.rota_max_acc = std::get<1>(param);
        request->cartesian_params.push_back(ddsParam);
    }
    // 发送异步请求，然后等待返回
    auto result_future =
        m_set_cart_rota_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_set_cart_rota_max_acc_client->remove_pending_request(result_future);
        return false;
    }
    auto response = result_future.get();
    return response->success;
}

bool H10wRosParams::get_joint_mech_limit(std::vector<controller::msg::JointParams> &joint_params)
{
    /*等待服务端上线*/
    while (
        !m_get_mech_limit_client->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    auto request =
        std::make_shared<controller::srv::GetJointMechanicalLimit::Request>();
    // 发送异步请求，然后等待返回
    auto result_future = m_get_mech_limit_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_mech_limit_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    joint_params = result->joint_params;
    return true;
}

bool H10wRosParams::get_joint_mech_max_vel(std::vector<controller::msg::JointParams> &joint_params)
{
    /*等待服务端上线*/
    while (
        !m_get_mech_max_vel_client->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request =
        std::make_shared<controller::srv::GetJointMechanicalMaxVel::Request>();
    // 发送异步请求，然后等待返回
    auto result_future = m_get_mech_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_mech_max_vel_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    joint_params = result->joint_params;
    return true;
}

bool H10wRosParams::get_joint_mech_max_acc(std::vector<controller::msg::JointParams> &joint_params)
{
    /*等待服务端上线*/
    while (
        !m_get_mech_max_acc_client->wait_for_service(std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request =
        std::make_shared<controller::srv::GetJointMechanicalMaxAcc::Request>();
    // 发送异步请求，然后等待返回
    auto result_future = m_get_mech_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_mech_max_acc_client->remove_pending_request(result_future);
        return false;
    }
    auto result = result_future.get();
    joint_params = result->joint_params;
    return true;
}

bool H10wRosParams::get_cart_mech_trans_max_vel(std::vector<controller::msg::CartesianParams> &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_get_cart_mech_trans_max_vel_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::GetCartesianMechanicalTranslationMaxVel::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_cart_mech_trans_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_cart_mech_trans_max_vel_client->remove_pending_request(
            result_future);
        return false;
    }
    auto result = result_future.get();
    cartesian_params = result->cartesian_params;
    return true;
}

bool H10wRosParams::get_cart_mech_trans_max_acc(std::vector<controller::msg::CartesianParams> &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_get_cart_mech_trans_max_acc_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::GetCartesianMechanicalTranslationMaxAcc::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_cart_mech_trans_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_cart_mech_trans_max_acc_client->remove_pending_request(
            result_future);
        return false;
    }
    auto result = result_future.get();
    cartesian_params = result->cartesian_params;
    return true;
}

bool H10wRosParams::get_cart_mech_rota_max_vel(std::vector<controller::msg::CartesianParams> &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_get_cart_mech_rota_max_vel_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::GetCartesianMechanicalRotationMaxVel::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_cart_mech_rota_max_vel_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_cart_mech_rota_max_vel_client->remove_pending_request(
            result_future);
        return false;
    }
    auto result = result_future.get();
    cartesian_params = result->cartesian_params;
    return true;
}

bool H10wRosParams::get_cart_mech_rota_max_acc(std::vector<controller::msg::CartesianParams> &cartesian_params)
{
    /*等待服务端上线*/
    while (!m_get_cart_mech_rota_max_acc_client->wait_for_service(
        std::chrono::seconds(1)))
    {
        // 等待时检测rclcpp的状态
        if (!rclcpp::ok())
        {
            RCLCPP_ERROR(this->get_logger(), "等待服务的过程中被打断...");
            return false;
        }
        RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
    }

    // 构造请求
    auto request = std::make_shared<
        controller::srv::GetCartesianMechanicalRotationMaxAcc::Request>();
    // 发送异步请求，然后等待返回
    auto result_future =
        m_get_cart_mech_rota_max_acc_client->async_send_request(request);
    if (rclcpp::spin_until_future_complete(this->shared_from_this(), result_future) !=
        rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_ERROR(this->get_logger(), "服务调用失败 :(");
        m_get_cart_mech_rota_max_acc_client->remove_pending_request(
            result_future);
        return false;
    }
    auto result = result_future.get();
    cartesian_params = result->cartesian_params;
    return true;
}