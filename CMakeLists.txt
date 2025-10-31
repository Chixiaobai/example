# 最外层 CMakeLists.txt（完全还原原逻辑）
cmake_minimum_required(VERSION 3.8)
project(h10w_controller_test)

# 1. 基础配置（原 h1_sdk_base.cmake 内容）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Environment/base)

# 2. 引入依赖（ROS 2、GTest、第三方库）
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(controller REQUIRED)
find_package(GTest REQUIRED)
find_package(ament_cmake_gtest REQUIRED)

# 第三方库（grpc、protobuf）
set(THIRDPART_DIR ${CMAKE_CURRENT_LIST_DIR}/thirdparty)
find_package(absl REQUIRED PATHS "${THIRDPART_DIR}/grpc/lib/cmake/absl" NO_DEFAULT_PATH)
find_package(utf8_range REQUIRED PATHS "${THIRDPART_DIR}/grpc/lib/cmake/utf8_range" NO_DEFAULT_PATH)
find_package(Protobuf REQUIRED PATHS "${THIRDPART_DIR}/grpc/lib/cmake/protobuf" NO_DEFAULT_PATH)
find_package(gRPC REQUIRED PATHS "${THIRDPART_DIR}/grpc/lib/cmake/grpc" NO_DEFAULT_PATH)
set(GRPC_LIB gRPC::grpc++_reflection gRPC::grpc++ protobuf::libprotobuf)

# 3. 测试程序配置（完全还原原逻辑）
## 测试用例源码（你的测试文件）
file(GLOB_RECURSE TEST_SRCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/H10W/grpc/*/*.cpp
)

## H10w 其他源码（若有额外文件，可继续添加）
file(GLOB_RECURSE H10W_SRCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include_H10w/*
)

## 头文件目录（包含所有需要的路径）
include_directories(
    ${H1_SDK_BASE_INCLUDE} 
    ${THIRDPART_DIR}/grpc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include_H10w
    ${CMAKE_CURRENT_SOURCE_DIR}/include_H10w/part_test
    ${CMAKE_CURRENT_SOURCE_DIR}/include_H10w/grpc_msg/grpc_ws/monitor/
    ${CMAKE_CURRENT_SOURCE_DIR}/include_H10w/grpc_msg/grpc_ws/common/
    ${CMAKE_CURRENT_SOURCE_DIR}/include_H10w/grpc_msg/fastdds_ws/src/controller/
    ${CMAKE_CURRENT_SOURCE_DIR}/include_H10w/rpc_service
    ${rclcpp_INCLUDE_DIRS}
)

# 4. 生成可执行文件（合并所有源码：基础源码+测试源码+H10w源码）
add_executable(${PROJECT_NAME}
    ${TEST_SRCS}           # 测试用例源码
    ${H10W_SRCS}           # H10w 其他源码（防止漏加）
)

# 5. 链接所有依赖（完全还原原逻辑）
target_link_libraries(${PROJECT_NAME}
    ${H1_SDK_BASE_LIB} # 框架基础依赖库
    ${GRPC_LIB}            # grpc 相关库
    ${rclcpp_LIBRARIES}    # ROS 2 rclcpp 库
    ${controller_LIBRARIES}# controller 库
    GTest::GTest           # GTest 库
    GTest::Main            # GTest 主程序
)

# 6. ROS 2 依赖传递
ament_target_dependencies(${PROJECT_NAME}
    rclcpp
    controller
)

# 7. 注册 GTest 测试目标
ament_add_gtest_test(${PROJECT_NAME})

# 8. 安装配置
install(TARGETS
    ${PROJECT_NAME}
    DESTINATION lib/${PROJECT_NAME}
)

# 9. 输出文件后缀
set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX "d"
    RELWITHDEBINFO_POSTFIX "rd"
    MINSIZEREL_POSTFIX "s"
)

ament_package()
