#pragma once

#include <rclcpp/rclcpp.hpp>
#include "controller/msg/move_message.hpp"
#include "controller/msg/error_message.hpp"
#include "H10Wglobalconstants.h"
#include "controller/msg/joint_params.hpp"
#include "controller/msg/cartesian_params.hpp"
#include "controller/srv/get_joint_max_vel.hpp"
#include "controller/srv/get_joint_max_acc.hpp"
#include "controller/srv/get_joint_soft_limit.hpp"
#include "controller/srv/get_cartesian_translation_max_vel.hpp"
#include "controller/srv/get_cartesian_translation_max_acc.hpp"
#include "controller/srv/get_cartesian_rotation_max_vel.hpp"
#include "controller/srv/get_cartesian_rotation_max_acc.hpp"
#include "controller/srv/get_joint_mechanical_limit.hpp"
#include "controller/srv/set_joint_max_vel.hpp"
#include "controller/srv/set_joint_max_acc.hpp"
#include "controller/srv/set_joint_soft_limit.hpp"
#include "controller/srv/set_cartesian_translation_max_vel.hpp"
#include "controller/srv/set_cartesian_translation_max_acc.hpp"
#include "controller/srv/set_cartesian_rotation_max_vel.hpp"
#include "controller/srv/set_cartesian_rotation_max_acc.hpp"
#include "controller/srv/get_joint_mechanical_max_vel.hpp"
#include "controller/srv/get_joint_mechanical_max_acc.hpp"
#include "controller/srv/get_cartesian_mechanical_translation_max_vel.hpp"
#include "controller/srv/get_cartesian_mechanical_translation_max_acc.hpp"
#include "controller/srv/get_cartesian_mechanical_rotation_max_vel.hpp"
#include "controller/srv/get_cartesian_mechanical_rotation_max_acc.hpp"
#include "H10Wglobalconstants.h"

using namespace GlobalConstants::H10W;
// joint_index, target_position, velocity
using JointTarget = std::tuple<uint32_t, double, double>;
// joint_index, max_pos, min_pos
using JointLimit = std::vector<std::tuple<uint32_t, double, double>>;
// index, max_value
using JointMaxAcc = std::vector<std::tuple<uint32_t, double>>;
using JointMaxVel = std::vector<std::tuple<uint32_t, double>>;
using CartRotaAcc = std::vector<std::tuple<uint32_t, double>>;
using CartRotaVel = std::vector<std::tuple<uint32_t, double>>;
using CartTransAcc = std::vector<std::tuple<uint32_t, double>>;
using CartTransVel = std::vector<std::tuple<uint32_t, double>>;

class H10wRosParams : public rclcpp::Node
{
public:
    explicit H10wRosParams(const std::string &strIpPort = "localhost");
    ~H10wRosParams() = default;

public:
    bool get_joint_soft_limit(std::vector<controller::msg::JointParams> &joint_params);
    bool get_joint_max_vel(std::vector<controller::msg::JointParams> &joint_params);
    bool get_joint_max_acc(std::vector<controller::msg::JointParams> &joint_params);
    bool get_cart_trans_max_vel(std::vector<controller::msg::CartesianParams> &cartesian_params);
    bool get_cart_trans_max_acc(std::vector<controller::msg::CartesianParams> &cartesian_params);
    bool get_cart_rota_max_vel(std::vector<controller::msg::CartesianParams> &cartesian_params);
    bool get_cart_rota_max_acc(std::vector<controller::msg::CartesianParams> &cartesian_params);
    bool set_joint_soft_limit(const JointLimit &joint_params);
    bool set_joint_max_vel(const JointMaxVel &joint_params);
    bool set_joint_max_acc(const JointMaxAcc &joint_params);
    bool set_cart_trans_max_vel(const CartTransVel &cartesian_params);
    bool set_cart_trans_max_acc(const CartTransAcc &cartesian_params);
    bool set_cart_rota_max_vel(const CartRotaVel &cartesian_params);
    bool set_cart_rota_max_acc(const CartRotaAcc &cartesian_params);
    bool get_joint_mech_limit(std::vector<controller::msg::JointParams> &joint_params);
    bool get_joint_mech_max_vel(std::vector<controller::msg::JointParams> &joint_params);
    bool get_joint_mech_max_acc(std::vector<controller::msg::JointParams> &joint_params);
    bool get_cart_mech_trans_max_vel(std::vector<controller::msg::CartesianParams> &cartesian_params);
    bool get_cart_mech_trans_max_acc(std::vector<controller::msg::CartesianParams> &cartesian_params);
    bool get_cart_mech_rota_max_vel(std::vector<controller::msg::CartesianParams> &cartesian_params);
    bool get_cart_mech_rota_max_acc(std::vector<controller::msg::CartesianParams> &cartesian_params);

protected:
    rclcpp::Client<controller::srv::GetJointSoftLimit>::SharedPtr
        m_get_soft_limit_client;
    rclcpp::Client<controller::srv::GetJointMaxVel>::SharedPtr
        m_get_joint_max_vel_client;
    rclcpp::Client<controller::srv::GetJointMaxAcc>::SharedPtr
        m_get_joint_max_acc_client;
    rclcpp::Client<controller::srv::GetCartesianTranslationMaxVel>::SharedPtr
        m_get_cart_trans_max_vel_client;
    rclcpp::Client<controller::srv::GetCartesianTranslationMaxAcc>::SharedPtr
        m_get_cart_trans_max_acc_client;
    rclcpp::Client<controller::srv::GetCartesianRotationMaxVel>::SharedPtr
        m_get_cart_rota_max_vel_client;
    rclcpp::Client<controller::srv::GetCartesianRotationMaxAcc>::SharedPtr
        m_get_cart_rota_max_acc_client;

    rclcpp::Client<controller::srv::SetJointSoftLimit>::SharedPtr
        m_set_soft_limit_client;
    rclcpp::Client<controller::srv::SetJointMaxVel>::SharedPtr
        m_set_joint_max_vel_client;
    rclcpp::Client<controller::srv::SetJointMaxAcc>::SharedPtr
        m_set_joint_max_acc_client;
    rclcpp::Client<controller::srv::SetCartesianTranslationMaxVel>::SharedPtr
        m_set_cart_trans_max_vel_client;
    rclcpp::Client<controller::srv::SetCartesianTranslationMaxAcc>::SharedPtr
        m_set_cart_trans_max_acc_client;
    rclcpp::Client<controller::srv::SetCartesianRotationMaxVel>::SharedPtr
        m_set_cart_rota_max_vel_client;
    rclcpp::Client<controller::srv::SetCartesianRotationMaxAcc>::SharedPtr
        m_set_cart_rota_max_acc_client;

    rclcpp::Client<controller::srv::GetJointMechanicalLimit>::SharedPtr
        m_get_mech_limit_client;
    rclcpp::Client<controller::srv::GetJointMechanicalMaxVel>::SharedPtr
        m_get_mech_max_vel_client;
    rclcpp::Client<controller::srv::GetJointMechanicalMaxAcc>::SharedPtr
        m_get_mech_max_acc_client;
    rclcpp::Client<controller::srv::GetCartesianMechanicalTranslationMaxVel>::
        SharedPtr m_get_cart_mech_trans_max_vel_client;
    rclcpp::Client<controller::srv::GetCartesianMechanicalTranslationMaxAcc>::
        SharedPtr m_get_cart_mech_trans_max_acc_client;
    rclcpp::Client<controller::srv::GetCartesianMechanicalRotationMaxVel>::
        SharedPtr m_get_cart_mech_rota_max_vel_client;
    rclcpp::Client<controller::srv::GetCartesianMechanicalRotationMaxAcc>::
        SharedPtr m_get_cart_mech_rota_max_acc_client;
};