#pragma once

#include <string>
#include <mutex>

//单例类
class Singleton {
protected:
    Singleton();
    virtual ~Singleton();

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    static void createInstance();
    static Singleton *getInstance();
    static void destroyInstance();
    
    //设置获取机械臂类型
    static void setRobotName(std::string name);
    static std::string getRobotName();

    //设置获取用例名称
    static void setTestCaseName(std::string name);
    static std::string getTestCaseName();


protected:
    //单例指针
    static Singleton *m_pInstance;
    //实例操作锁
    static std::mutex instance_mutex;

    static std::string m_strRobotName;

    static std::string m_strTestCaseName;
};

#define SetRobotType(name) Singleton::getInstance()->setRobotName(name)

#define GetRobotType() Singleton::getInstance()->getRobotName()

#define SetTestCaseType(name) Singleton::getInstance()->setTestCaseName(name)

#define GetTestCaseType() Singleton::getInstance()->getTestCaseName()
