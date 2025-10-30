#include <signal.h>
#include "H10wGrpcMove.h"
#include "h1_sdk_base.h"

static H10wGrpcMove *g_pTester = nullptr;

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
    if (sigaction(SIGINT, &stSigAction, nullptr) != 0 || sigaction(SIGTERM, &stSigAction, nullptr) != 0)
    {
        printf("Fail to set callback function for console application!\n");
        fflush(stdout);
    }
}

// 定义测试用例的描述, 方便用户了解测试内容
void H10w_FT_Grpc_Params_003() { printf("验证设置关节软限位函数的有效性\n"); }

// 定义测试实体，多个用例可以关联同一个实体
void h10w_ft_grpc_params_003()
{

    setConsoleHandler();

    std::vector<std::string> context; // 测试任务内容
    std::vector<bool> num;            // 测试任务结果
    std::vector<std::string> vec;     // 存储错误信息容器

    auto node = std::make_shared<H10wGrpcMove>(IpPort);

    // 启动spin循环（单独线程，避免阻塞主逻辑）
    std::thread spin_thread([&node]()
                            { rclcpp::spin(node); });

    // 获取所有关节软限位
    std::cout << "Get Joint Soft Limits: " << std::endl;
    auto soft_limits = node->m_pControllerClient->getJointSoftLimit();
    for (auto &[i, max, min] : soft_limits)
    {
        std::cout << i << " = [max:" << max << "; min:" << min << ";]" << std::endl;
    }

    // 测试任务1：设置关节软限位
    context.emplace_back("修改所有关节软限位");
    std::cout << "Modify Joint Soft Limits: " << std::endl;
    int joint_count = 19;
    std::vector<int> joint_index = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    std::vector<double> min_pos = {-0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.2, -0.2, 0.1, -0.001, -0.001};
    std::vector<double> max_pos = {0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 0.2, 0.2, 0.6, 0.001, 0.001};

    node->m_pControllerClient->setJointSoftLimit(joint_count, joint_index, min_pos, max_pos);

    char ret = read_input("查看配置文件中的软限位，是否与设置的一致？(y/n)\n");
    Test_Task_Result(num, ret, 'y');
    // 测试任务2：恢复关节软限位
    context.emplace_back("恢复所有关节软限位");
    for (auto &[i, max, min] : soft_limits)
    {

        max_pos[i] = max;
        min_pos[i] = min;
    }

    node->m_pControllerClient->setJointSoftLimit(joint_count, joint_index, min_pos, max_pos);
    ret = read_input("查看配置文件中的软限位，是否与设置的一致？(y/n)\n");
    Test_Task_Result(num, ret, 'y');

    node->stopTest();
    // 等待spin线程结束
    spin_thread.join();

    // 解析测试结果
    Analysis_Test_Task_Result(num, "H10w_FT_Grpc_Params_003");
    if (num.at(num.size() - 1))
    {
        setCaseSucceed("H10w_FT_Grpc_Params_003");
    }
    else
    {
        setCaseFailed("H10w_FT_Grpc_Params_003");
    }

    sleepMilliseconds(1000);
}

// 注册测试用例及测试方法
REGIST_CASE_FUNCTION(h10w_ft_grpc_params_003)
REGIST_CASE(H10w_FT_Grpc_Params_003, h10w_ft_grpc_params_003, H10w_FT_Grpc_Params_003);