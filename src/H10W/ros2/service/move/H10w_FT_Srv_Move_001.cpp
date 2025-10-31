#include <signal.h>
#include "H10wRos2Client.h"
#include "main.h"

static H10wRos2Client *g_pTester = nullptr;

static void consoleHandler(int intSigNum)
{
    if ((SIGINT == intSigNum) || (SIGTERM == intSigNum))
    {
        if (nullptr != g_pTester)
        {
            g_pTester->stopTest();
        }
    }
}

static void setConsoleHandler()
{
    struct sigaction stSigAction;
    stSigAction.sa_handler = &consoleHandler;
    sigemptyset(&stSigAction.sa_mask);
    stSigAction.sa_flags = 0;
    if (sigaction(SIGINT, &stSigAction, nullptr) != 0 ||
        sigaction(SIGTERM, &stSigAction, nullptr) != 0)
    {
        printf("Fail to set callback function for console application!\n");
        fflush(stdout);
    }
}

GTEST_CASE(auto_Srv_Move, H10w_FT_Srv_Move_001, "验证单关节运动")
{

    setConsoleHandler();

    auto test_context = std::make_shared<rclcpp::Context>();
    test_context->init(0, nullptr);

    auto node = std::make_shared<H10wRos2Client>(IpPort);
    g_pTester = node.get();

    // 启动spin循环（单独线程，避免阻塞主逻辑）
    std::thread spin_thread([&node]()
                            { rclcpp::spin(node); });

    std::vector<controller::msg::JointParams> joint_params;
    auto &params_module = node->get_params_module();
    if (params_module.get_joint_soft_limit(joint_params))
    {
        std::cout << "get_joint_soft_limit succeed!" << std::endl;
        for (const auto &param : joint_params)
        {
            std::cout << "joint index: " << param.joint_index;
            std::cout << "max limit: " << param.max_pos;
            std::cout << "min limit: " << param.min_pos << std::endl;
        }
    }
    else
    {
        std::cout << "get_joint_soft_limit failed!" << std::endl;
    }

    node->stopTest();
    // 等待spin线程结束
    spin_thread.join();

    // 解析测试结果
    Analysis_Test_Task_Result(num, "H10w_FT_SpeedMonitor_Elevator_001");
    if (num.at(num.size() - 1))
    {
        setCaseSucceed("H10w_FT_SpeedMonitor_Elevator_001");
    }
    else
    {
        setCaseFailed("H10w_FT_SpeedMonitor_Elevator_001");
    }

    sleepMilliseconds(1000);
}

// 注册测试用例及测试方法
REGIST_CASE_FUNCTION(h10w_ft_speedmonitor_elevator_001)
REGIST_CASE(H10w_FT_SpeedMonitor_Elevator_001, h10w_ft_speedmonitor_elevator_001, H10w_FT_SpeedMonitor_Elevator_001);