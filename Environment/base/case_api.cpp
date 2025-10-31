#include "case_api.h"
#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <string>
std::string g_target_group_prefix = "";

bool should_skip_test(const std::string& current_group_name) {
    // 场景1：输入了-case（指定执行范围）→ 不跳过任何用例（GTest会自动过滤）
    if (!g_target_group_prefix.empty()) {
        return false;
    }

    // 场景2：未输入-case（默认执行auto组）→ 非auto组的用例需要跳过
    if (current_group_name.substr(0, 4) != "auto") {  // 判断组名是否以"auto"开头
        return true;
    }

    return false;
}

// 解析命令行参数（保留原有逻辑，兼容-case/-robot参数）
bool parse_test_arguments(int argc, char**argv, TestTaskConfig& config) {
    // 初始化默认过滤规则（执行所有用例）
    config.case_filter = "auto*";  // 不输入-case时的默认值
    g_target_group_prefix = "auto";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-case" && i + 1 < argc) {
            // 解析-case参数：支持 "BATCH_NAME" 或 "BATCH_NAME.CASE_NAME"
            std::string case_input = argv[++i];
            config.case_type = case_input;  // 保存入参的-case值
            g_target_group_prefix = "";
            // 生成GTest过滤规则：
            if (case_input.find('.') != std::string::npos) {
                // 输入单个用例（如"H10w_FT_Grpc_Params.Params_001"）
                config.case_filter = case_input;
            } else {
                // 输入批次名（如"H10w_FT_Grpc_Params"），执行该批次下所有用例
                config.case_filter = case_input + ".*";
            }
        } else if (arg == "-robot" && i + 1 < argc) {
            config.robot_type = argv[++i];
        } else if (arg == "-report" && i + 1 < argc) {
            config.report_path = argv[++i];
        }
        // 忽略其他参数（如--gtest_filter，由程序自动生成）
    }

    // 校验必填参数（仅robot_type必填）
    if (config.robot_type.empty()) {
        std::cerr << "错误：必须使用-robot指定机器人类型（如-robot H10w）" << std::endl;
        return false;
    }

    return true;
}



// 将case_type转换为GTest过滤表达式（如"FT_grpc" → "H10w_FT*"）
std::string case_type_to_filter(const std::string &case_type, const std::string &robot_type)
{
    // 提取case_type前缀（如"FT_grpc"取"FT"，"Joint_Test"取"Joint"）
    size_t under_pos = case_type.find('_');
    std::string case_prefix = (under_pos != std::string::npos)
                                  ? case_type.substr(0, under_pos)
                                  : case_type;
    // 生成过滤规则：机器人类型_前缀*（确保只匹配目标用例组）
    return robot_type + "_" + case_prefix + "*";
}

// 加载测试配置文件（根据robot_type和case_type自动定位配置）
std::map<std::string, std::string> load_test_config(
    const std::string &case_type,
    const std::string &robot_type,
    const std::string &config_path)
{
    std::map<std::string, std::string> params;
    // 自动生成配置路径（未传路径时默认：config/机器人类型/用例类型.xml）
    std::string actual_path = config_path.empty()
                                  ? "config/" + robot_type + "/" + case_type + ".xml"
                                  : config_path;

    // 打印配置加载日志（实际项目中需替换为XML/JSON解析逻辑）
    std::cout << "[配置加载] 路径：" << actual_path << std::endl;
    // 模拟配置参数（实际需从文件读取，此处仅为示例）
    params["expected_param"] = "0.01";    // 示例：预期参数值
    params["timeout"] = "5000";           // 示例：超时时间（毫秒）
    params["robot_ip"] = "192.168.1.100"; // 示例：机器人IP

    return params;
}

// 生成HTML格式报告（图形化，支持浏览器打开）s
bool save_html_report(const std::string &report_path, const TestTaskConfig &config)
{
    // 校验报告路径
    if (report_path.empty())
    {
        std::cerr << "警告：HTML报告路径为空，无法生成" << std::endl;
        return false;
    }

    // 打开文件（若不存在则创建，存在则覆盖）
    std::ofstream ofs(report_path);
    if (!ofs.is_open())
    {
        std::cerr << "错误：无法打开HTML报告文件（路径：" << report_path << "）" << std::endl;
        return false;
    }

    // 1. HTML头部（引入样式，确保美观且响应式）
    ofs << R"(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>测试报告（HTML版）</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: "Microsoft YaHei", Arial, sans-serif; background-color: #f5f5f5; padding: 20px; }
        .report-wrapper { max-width: 1200px; margin: 0 auto; background: white; border-radius: 8px; box-shadow: 0 2px 12px rgba(0,0,0,0.08); overflow: hidden; }
        .report-header { background-color: #2c3e50; color: white; padding: 30px; text-align: center; }
        .header-title { font-size: 24px; margin-bottom: 15px; }
        .header-info { font-size: 14px; line-height: 1.6; opacity: 0.9; }
        .stats-container { display: flex; justify-content: center; gap: 25px; padding: 30px; flex-wrap: wrap; background-color: #fafafa; border-bottom: 1px solid #eee; }
        .stat-card { background-color: white; padding: 20px 30px; border-radius: 6px; box-shadow: 0 1px 4px rgba(0,0,0,0.05); min-width: 140px; text-align: center; }
        .stat-label { font-size: 14px; color: #666; margin-bottom: 8px; }
        .stat-value { font-size: 28px; font-weight: bold; }
        .stat-total { color: #3498db; }
        .stat-passed { color: #2ecc71; }
        .stat-failed { color: #e74c3c; }
        .stat-skipped { color: #f39c12; }
        .suites-container { padding: 30px; }
        .suite-card { margin-bottom: 25px; border: 1px solid #eee; border-radius: 6px; overflow: hidden; }
        .suite-header { background-color: #f8f8f8; padding: 15px 20px; font-size: 16px; font-weight: bold; color: #333; border-bottom: 1px solid #eee; }
        .case-list { padding: 10px 0; }
        .case-item { padding: 12px 20px; border-bottom: 1px solid #f5f5f5; display: flex; align-items: center; justify-content: space-between; }
        .case-item:last-child { border-bottom: none; }
        .case-pass { color: #27ae60; }
        .case-fail { color: #c0392b; }
        .case-icon { font-size: 18px; margin-right: 12px; }
        .case-name { font-size: 14px; flex: 1; }
        .case-time { font-size: 13px; color: #999; }
        .fail-detail { padding: 12px 20px; background-color: #fdf2f2; border-left: 4px solid #e74c3c; margin: 0 20px 15px; border-radius: 4px; font-size: 14px; color: #c0392b; }
        .report-footer { background-color: #fafafa; padding: 15px 20px; text-align: center; font-size: 13px; color: #999; border-top: 1px solid #eee; }
    </style>
</head>
<body>
    <div class="report-wrapper">
        <!-- 报告头部 -->
        <div class="report-header">
            <div class="header-title">测试报告</div>
            <div class="header-info">测试时间：)"
        << get_current_time_str() << R"(</div>
            <div class="header-info">臂型：)"
        << config.robot_type << R"(</div>
        </div>

        <!-- 统计卡片区 -->
        <div class="stats-container">
            <div class="stat-card">
                <div class="stat-label">总用例数</div>
                <div class="stat-value stat-total">)"
        << testing::UnitTest::GetInstance()->total_test_count() << R"(</div>
            </div>
            <div class="stat-card">
                <div class="stat-label">通过</div>
                <div class="stat-value stat-passed">)"
        << testing::UnitTest::GetInstance()->successful_test_count() << R"(</div>
            </div>
            <div class="stat-card">
                <div class="stat-label">失败</div>
                <div class="stat-value stat-failed">)"
        << testing::UnitTest::GetInstance()->failed_test_count() << R"(</div>
            </div>
            <div class="stat-card">
                <div class="stat-label">跳过</div>
                <div class="stat-value stat-skipped">)"
        << testing::UnitTest::GetInstance()->disabled_test_count() << R"(</div>
            </div>
        </div>

        <!-- 测试组详情区 -->
        <div class="suites-container">
    )";

    // 2. 遍历所有测试组（适配旧版本GTest：用test_suite_count()）
    const testing::UnitTest *unit_test = testing::UnitTest::GetInstance();
    for (int suite_idx = 0; suite_idx < unit_test->total_test_suite_count(); ++suite_idx)
    {
        const testing::TestSuite *suite = unit_test->GetTestSuite(suite_idx);
        // 跳过空测试组（无可用用例）
        if (suite->total_test_count() == 0)
            continue;

        ofs << R"(
            <div class="suite-card">
                <div class="suite-header">
                    测试模块：)"
            << suite->name() << R"( 
                    （通过：)"
            << suite->successful_test_count() << R"(/总用例数：)" << suite->total_test_count() << R"(）
                </div>
                <div class="case-list">
        )";

        // 遍历组内每个用例
        for (int case_idx = 0; case_idx < suite->total_test_count(); ++case_idx)
        {
            const testing::TestInfo *test_case = suite->GetTestInfo(case_idx);
            const testing::TestResult *case_result = test_case->result();
            bool is_pass = case_result->Passed();

            // 写入用例基本信息
            ofs << R"(
                    <div class="case-item )"
                << (is_pass ? "case-pass" : "case-fail") << R"(">
                        <div class="case-icon">)"
                << (is_pass ? "✅" : "❌") << R"(</div>
                        <div class="case-name">)"
                << test_case->name() << R"(</div>
                        <div class="case-time">耗时：)"
                << std::fixed << std::setprecision(3)
                << case_result->elapsed_time() / 1000.0 << R"(秒</div>
                    </div>
            )";

            // 写入失败详情（适配旧版本GTest：用GetTestPartResult()）
            if (!is_pass && case_result->total_part_count() > 0)
            {
                const testing::TestPartResult &fail_part = case_result->GetTestPartResult(0);
                ofs << R"(
                    <div class="fail-detail">
                        失败原因：)"
                    << fail_part.message() << R"(
                    </div>
                )";
            }
        }

        ofs << R"(
                </div>
            </div>
        )";
    }

    // 3. HTML尾部
    ofs << R"(
        </div>
        <div class="report-footer">
            报告生成时间：)"
        << get_current_time_str() << R"( | 生成工具：GTest HTML Reporter
        </div>
    </div>
</body>
</html>
    )";

    // 关闭文件并打印日志
    ofs.close();
    std::cout << "[报告生成] HTML报告已保存至：" << report_path << std::endl;
    return true;
}

// 获取当前时间字符串（Linux系统专用，格式：YYYY-MM-DD HH:MM:SS），线程安全
std::string get_current_time_str()
{
    // 1. 获取当前系统时间（秒级精度）
    std::time_t now_time_t = std::time(nullptr);

    // 2. Linux线程安全的本地时间转换（替换非线程安全的localtime）
    std::tm local_tm{};
    localtime_r(&now_time_t, &local_tm); // Linux专用线程安全接口

    // 3. 格式化时间（HH:MM:SS自动补零，如08:05:03）
    char time_buf[64] = {0};
    std::strftime(
        time_buf,
        sizeof(time_buf),
        "%Y-%m-%d %H:%M:%S", // 格式符：%H(24小时制)、%M(分钟)、%S(秒)
        &local_tm);

    return time_buf;
}
// // 从测试用例路径提取文件夹名称（支持Linux路径格式）
// std::string extract_case_folder_name(const std::string& test_case_path) {
//     // 找到最后一个 "/" 的位置（路径分隔符）
//     size_t last_slash_pos = test_case_path.find_last_of('/');
//     if (last_slash_pos == std::string::npos) {
//         return "unknown_case_folder";  // 无路径分隔符时返回默认值
//     }

//     // 找到倒数第二个 "/" 的位置（获取最后一级文件夹）
//     size_t second_last_slash_pos = test_case_path.find_last_of('/', last_slash_pos - 1);
//     if (second_last_slash_pos == std::string::npos) {
//         // 只有一级路径（如"/params/xxx.cpp"），取第一个 "/" 到第二个 "/" 之间的内容
//         return test_case_path.substr(1, last_slash_pos - 1);
//     }

//     // 提取倒数第二个 "/" 到最后一个 "/" 之间的内容（即最后一级文件夹名）
//     return test_case_path.substr(second_last_slash_pos + 1, last_slash_pos - second_last_slash_pos - 1);
// }
