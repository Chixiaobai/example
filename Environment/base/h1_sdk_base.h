#pragma once

#ifdef _WIN32
#include "windows.h"
#else
#include "time.h"
#endif
#include <rclcpp/rclcpp.hpp>
#include "string.h"
#include "float.h"
#include "math.h"
#include "iostream"
#include "fstream"
#include "case_api.h"
#include <vector>
#include "singleton.h"
#include "xmlHandler.h"
#include <gtest/gtest.h>

template <typename T>
bool less_than(const T& a, const T& b) {
    if (a < b) return true;
    return false;
}

template <typename T>
bool more_than(const T& a, const T& b) {
    if (a > b) return true;
    return false;
}

template <typename T>
bool equal(const T& a, const T& b) {
    if (a == b) return true;
    return false;
}

template <typename T>
bool not_equal(const T& a, const T& b) {
    if (a != b) return true;
    return false;
}

#define Test_Task_Result_Ex(num, ret, op, success_value_list)                                \
    {                                                                                        \
        vec.emplace_back(std::string(__FILE__) + ":" + std::to_string(__LINE__ - 1) + "行"); \
        bool flag = false;                                                                   \
        for (auto item : success_value_list) {                                               \
            if (op(ret, item)) {                                                             \
                flag = true;                                                                 \
            } else {                                                                         \
                flag = false;                                                                \
                break;                                                                       \
            }                                                                                \
        }                                                                                    \
        num.emplace_back(flag);                                                              \
        std::cout<< "第" <<  num.size() << "个测试任务" <<"actual test value:" << ret << std::endl;     \
    }

#define Test_Task_Result(num, ret, success_value)                                            \
    {                                                                                        \
        vec.emplace_back(std::string(__FILE__) + ":" + std::to_string(__LINE__ - 1) + "行"); \
        num.emplace_back((ret == success_value));                                            \
        std::cout<< "第" <<  num.size() << "个测试任务" <<"actual test value:" << ret <<  std::endl;    \
    }

#define Analysis_Test_Task_Result(num, strCaseId)                                                    \
    {                                                                                                \
        ofstream outFile;                                                                            \
        outFile.open("Auto_test_result_explanation.txt", ios::app);                                  \
        outFile << "********测试" << strCaseId << "用例********" << endl;                        \
        bool bHasError = false;                                                                      \
        for (int i = 0; i < num.size(); ++i) {                                                       \
            if (!num[i]) {                                                                            \
                bHasError = true;                                                                    \
                outFile << "第" << i + 1 << "个测试任务执行失败" << endl; \
                outFile << "第" << i + 1 << "个测试任务的测试内容:" << context.at(i).c_str() << endl; \
                outFile << "第" << i + 1 << "个测试任务错误所在行:" << vec.at(i).c_str() << endl; \
            }                                                                                        \
        }                                                                                            \
        if(!bHasError)                                                                               \
        {                                                                                            \
            outFile << "测试" << strCaseId << "用例成功" << endl;                                      \
        }                                                                                            \
        outFile.close();                                                                             \
        num.emplace_back(!bHasError);                                                                \
    }

#define Get_robot_config_file_path(robot_point_file)\
    int pos1=std::string(__FILE__).rfind("/");\
    int pos2=std::string(__FILE__).rfind(".");\
    robot_point_file=std::string(__FILE__).substr(0,pos1+1)+"config/"+"config.xml";\
    std::cout << "robot_config_file:" << robot_point_file.c_str() << std::endl; \

// 角度转换：弧度-->度
float to_degree(float x);

float to_rad(float x);

// 线程睡眠
void sleepMilliseconds(unsigned int uintMilliseconds);

// 等待用户输入一个字符
char read_input(const char* strPrompt);

// //获取控制箱IP
// char *get_strIpAddress();

//读取数据
vector<vector<double>> read_data(string file_name);

//浮点数是否相等
bool isFloatValueEqual(float fltValueLeft, float fltValueRight, float fltEpsilon = 0.01f);

// 时间转换函数，将 index 转换为 mm:ss:zzz 格式的时间
std::string indexToTime(int ms);

//缩放正弦函数
float Sin(float index,float lowerlimit,float upperlimit,float samplecount);


// 判断输入值是否在指定的上下限范围内
bool isWithinRange(double inputValue, double lowerBound, double upperBound);

// std::string indexToTime(int index);

//获取测试所需参数
std::map<std::string, std::vector<float>> getTestParameters(std::string &config_file_path);