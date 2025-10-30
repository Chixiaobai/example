#pragma once
#include "rclcpp/rclcpp.hpp"

#include "H10wRosMove.h"
#include "H10wRosParams.h"

class H10wRos2Client : public rclcpp::Node
{
public:
  explicit H10wRos2Client(const std::string &strIpPort);
  ~H10wRos2Client() = default;

  H10wRosMove &get_motion_module();
  H10wRosParams &get_params_module();

  void stopTest();

private:
  std::string m_strIpPort;
  std::unique_ptr<H10wRosMove> m_motion_module;
  std::unique_ptr<H10wRosParams> m_params_module;
};