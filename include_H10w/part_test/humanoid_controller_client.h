
#include <vector>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/config_protobuf.h>
#include "../grpc_msg/grpc_ws/common/grpc_pb_cpp/motion_service.grpc.pb.h"
#include "../grpc_msg/grpc_ws/common/grpc_pb_cpp/motion_service.pb.h"
#include "../grpc_msg/grpc_ws/common/grpc_pb_cpp/system_service.grpc.pb.h"
#include "../grpc_msg/grpc_ws/common/grpc_pb_cpp/system_service.pb.h"
#include "../grpc_msg/grpc_ws/common/grpc_pb_cpp/params_service.grpc.pb.h"
#include "../grpc_msg/grpc_ws/common/grpc_pb_cpp/params_service.pb.h"

using controller::CartesianParams;
using controller::GetCartesianParamsResponse;
using controller::GetJointParamsResponse;
using controller::JointParams;
using controller::ParamsService;
using controller::SetCartesianParamsRequest;
using controller::SetJointParamsRequest;
using controller::SetParamResponse;
using google::protobuf::Empty;

using controller::ErrorClearResponse;
using controller::JointAngle;
using controller::JointMoveRequest;
using controller::JointMoveResponse;
using controller::LinearMoveRequest;
using controller::LinearMoveResponse;
using controller::LinearMove;
using controller::MotionService;
using controller::MultiJointMoveRequest;
using controller::MultiJointMoveResponse;
using controller::StopResponse;
using controller::SystemService;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class HumanoidControllerClient final
{
public:
    HumanoidControllerClient(std::shared_ptr<Channel> pChannel);
    ~HumanoidControllerClient() = default;

    bool MultiJointsMove(int32_t jointcount, const std::vector<int32_t> &joint_index, const std::vector<float> &position, const std::vector<float> &velocity_percent,uint32_t &token);
    
    bool SingleJointMove(int32_t index, float &position, float &velocity_percent,uint32_t &token);

    bool LinearMovel(int32_t count, const std::vector<int32_t> &type, std::vector<std::vector<double>> &pose, const std::vector<float> velocity_percent, std::vector<float> acceleration_percent, int32_t &task_id);

    bool stop();

    bool GetVersion(std::vector<std::string> &version);

    bool ClearError();

    std::vector<std::tuple<uint32_t, double, double>> getJointSoftLimit();

    std::vector<std::pair<uint32_t, double>> getJointMaxVel();

    std::vector<std::pair<uint32_t, double>> getJointMaxAcc();

    std::vector<std::pair<uint32_t, double>> getCartesianTranslationMaxVel();

    std::vector<std::pair<uint32_t, double>> getCartesianTranslationMaxAcc();

    std::vector<std::pair<uint32_t, double>> getCartesianRotationMaxVel();

    std::vector<std::pair<uint32_t, double>> getCartesianRotationMaxAcc();

    bool setJointSoftLimit(int32_t jointcount, const std::vector<uint32_t> &joint_index, const std::vector<double> &maxPos, const std::vector<double> &minPos);

    bool setJointMaxVel(int32_t jointcount, const std::vector<uint32_t> &joint_index, const std::vector<double> &maxVel);

    bool setJointMaxAcc(int32_t jointcount, const std::vector<uint32_t> &joint_index, const std::vector<double> &maxAcc);

    bool setCartesianTranslationMaxVel(int32_t jointcount, const std::vector<uint32_t> &cartesian_index, const std::vector<double> &transMaxVel);

    bool setCartesianTranslationMaxAcc(int32_t jointcount, const std::vector<uint32_t> &cartesian_index, const std::vector<double> &transMaxAcc);

    bool setCartesianRotationMaxVel(int32_t jointcount, const std::vector<uint32_t> &cartesian_index, const std::vector<double> &rotaMaxVel);

    bool setCartesianRotationMaxAcc(int32_t jointcount, const std::vector<uint32_t> &cartesian_index, const std::vector<double> &rotaMaxAcc);

    std::vector<std::tuple<uint32_t, double, double>> getJointMechanicalLimit();

    std::vector<std::pair<uint32_t, double>> getJointMechanicalMaxVel();

    std::vector<std::pair<uint32_t, double>> getJointMechanicalMaxAcc();

    std::vector<std::pair<uint32_t, double>> getCartesianMechanicalTranslationMaxVel();

    std::vector<std::pair<uint32_t, double>> getCartesianMechanicalTranslationMaxAcc();

    std::vector<std::pair<uint32_t, double>> getCartesianMechanicalRotationMaxVel();

    std::vector<std::pair<uint32_t, double>> getCartesianMechanicalRotationMaxAcc();

private:
    std::unique_ptr<MotionService::Stub> m_motion_stub_;
    std::unique_ptr<SystemService::Stub> m_system_stub_;
    std::unique_ptr<ParamsService::Stub> m_params_stub_;
};