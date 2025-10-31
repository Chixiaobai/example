# Environment/base/CMakeLists.txt（子模块，仅处理base目录源码）
cmake_minimum_required(VERSION 3.8)
project(h1_sdk_base LANGUAGES CXX)

# 1. 基础配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. 查找rclcpp（base目录源码依赖ROS 2）
find_package(rclcpp REQUIRED)

# 3. 收集base目录下的所有.cpp（用file(GLOB)解析通配符，避免漏文件）
file(GLOB BASE_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp  # 匹配base目录下所有.cpp
)

# 4. 编译为静态库（供外层工程链接）
add_library(${PROJECT_NAME} STATIC ${BASE_SRC})

# 5. 链接base依赖的库（rclcpp、系统库等）
target_link_libraries(${PROJECT_NAME}
    dl
    rt
    pthread
    pugixml
    stdc++fs
    ${rclcpp_LIBRARIES}  # base源码依赖rclcpp
)

# 6. 向外层暴露头文件目录（确保外层能找到base的.h文件）
target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}  # base自身头文件
        ${rclcpp_INCLUDE_DIRS}       # rclcpp头文件（base源码需要）
)

# 7. 定义宏（供外层工程使用）
target_compile_definitions(${PROJECT_NAME}
    PUBLIC -D_USE_MATH_DEFINES -D_DISABLE_EXTENDED_ALIGNED_STORAGE -DUSE_GTEST
)

# 8. 向外层传递变量（库名、头文件目录，供外层链接）
set(H1_SDK_BASE_LIB ${PROJECT_NAME} PARENT_SCOPE)
set(H1_SDK_BASE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
