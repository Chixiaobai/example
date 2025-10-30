#include "H10wRos2Client.h"

H10wRos2Client::H10wRos2Client(const std::string &strIpPort)
    : Node(strIpPort),
      m_strIpPort(strIpPort),
      m_motion_module(std::make_unique<H10wRosMove>(strIpPort)),
      m_params_module(std::make_unique<H10wRosParams>(strIpPort))
{
}

H10wRosMove &H10wRos2Client::get_motion_module()
{
    return *m_motion_module;
}

H10wRosParams &H10wRos2Client::get_params_module()
{
    return *m_params_module;
}

void H10wRos2Client::stopTest()
{
    rclcpp::shutdown();
}