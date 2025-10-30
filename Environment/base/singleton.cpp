#include "singleton.h"

//静态成员变量初始化
Singleton *Singleton::m_pInstance = nullptr;
std::mutex Singleton::instance_mutex;
std::string Singleton::m_strRobotName;
std::string Singleton::m_strTestCaseName;


Singleton::Singleton() {}

Singleton::~Singleton() {}

void Singleton::createInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex);
    if (m_pInstance == nullptr) {
        m_pInstance = new Singleton();
    }
}

Singleton *Singleton::getInstance() { 
    std::lock_guard<std::mutex> lock(instance_mutex);
    return m_pInstance; }

void Singleton::destroyInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex);
    if (m_pInstance != nullptr) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
    }

void Singleton::setRobotName(std::string name){
    m_strRobotName = name;}

std::string Singleton::getRobotName() { 
    return m_strRobotName;}

void Singleton::setTestCaseName(std::string name){
    m_strTestCaseName = name;}

std::string Singleton::getTestCaseName() { 
    return m_strTestCaseName;}