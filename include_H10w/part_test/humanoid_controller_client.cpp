#include "humanoid_controller_client.h"

HumanoidControllerClient::HumanoidControllerClient(std::shared_ptr<grpc::Channel> pChannel)
    : m_motion_stub_(MotionService::NewStub(pChannel)),
      m_system_stub_(SystemService::NewStub(pChannel)),
      m_params_stub_(ParamsService::NewStub(pChannel)) {}

bool HumanoidControllerClient::SingleJointMove(int32_t index, float &position, float &velocity_percent, uint32_t &token)
{
    JointMoveRequest request;
    request.set_joint_index(index);
    request.set_target_position(position);
    request.set_velocity(velocity_percent);

    JointMoveResponse response;
    ClientContext ctx;
    Status status = m_motion_stub_->RequestSingleJointMove(&ctx, request, &response);
    if (status.ok())
    {
        token = response.token();
        std::cout << "SingleJointMove succeeded: " << response.success() << ", Token: " << token << std::endl;
        return response.success();
    }
    else
    {
        std::cerr << "SingleJointMove RPC failed: " << status.error_message() << '\n';
        return false;
    }
}

bool HumanoidControllerClient::MultiJointsMove(int32_t jointcount, const std::vector<int32_t> &joint_index, const std::vector<float> &position, const std::vector<float> &velocity_percent, uint32_t &token)
{
    if (jointcount <= 0)
    {
        std::cout << "Invalid joint count: " << jointcount << std::endl;
        return false;
    }
    if (joint_index.size() != jointcount ||
        position.size() != jointcount ||
        velocity_percent.size() != jointcount)
    {
        std::cout << "Parameter array size mismatch with joint count" << std::endl;
        return false;
    }
    MultiJointMoveRequest request;
    for (int i = 0; i < jointcount; i++)
    {
        JointAngle *joint = request.add_joint_angles();
        joint->set_joint_index(joint_index[i]);
        joint->set_target_position(position[i]);
        joint->set_velocity(velocity_percent[i]);
    }

    MultiJointMoveResponse response;
    ClientContext ctx;
    Status status = m_motion_stub_->RequestMultiJointsMove(&ctx, request, &response);
    if (status.ok())
    {
        token = response.token();
        std::cout << "MultiJointsMove succeeded: " << response.success() << ", Token: " << token << std::endl;
        return response.success();
    }
    else
    {
        std::cerr << "MultiJointsMove RPC failed: " << status.error_message() << '\n';
        return false;
    }
}

bool HumanoidControllerClient::LinearMovel(int32_t count, const std::vector<int32_t> &type, std::vector<std::vector<double>> &pose, const std::vector<float> velocity_percent, std::vector<float> acceleration_percent, int32_t &task_id)
{
    LinearMoveRequest request;
    for (int i = 0; i < count; i++)
    {
        LinearMove *Linear = request.add_linear_move();
        Linear->set_type(type[i]);
        for (int j = 0; j < 6; ++j)
        {
            Linear->add_pose(pose[i][j]);
        }
        Linear->set_velocity_percent(velocity_percent[i]);
        Linear->set_acceleration_percent(acceleration_percent[i]);
    }
    LinearMoveResponse response;
    ClientContext ctx;
    Status status = m_motion_stub_->RequestLinearMove(&ctx, request, &response);
    if (status.ok())
    {
        task_id = response.task_id();
        std::cout << "LinearMove succeeded: " << response.result() << ", task_id: " << task_id << std::endl;
        return response.result();
    }
    else
    {
        std::cerr << "LinearMove RPC failed: " << status.error_message() << '\n';
        return false;
    }
}
bool HumanoidControllerClient::stop()
{
    ClientContext ctx;
    google::protobuf::Empty empty;
    StopResponse response;
    Status status = m_motion_stub_->RequestStop(&ctx, empty, &response);
    if (!status.ok())
    {
        std::cerr << "Stop RPC failed: " << status.error_message() << '\n';
        return false;
    }
    return true;

    std::cout << "Stop success = " << std::boolalpha << response.success() << '\n';
}
bool HumanoidControllerClient::GetVersion(std::vector<std::string> &version)
{
    ClientContext ctx;
    google::protobuf::Empty empty;
    controller::VersionResponse response;
    Status status = m_system_stub_->GetVersion(&ctx, empty, &response);
    if (!status.ok())
    {
        std::cerr << "GetVersion RPC failed: " << status.error_message() << '\n';
        return false;
    }
    version.clear();
    version.push_back(response.main());
    for (const auto &[k, v] : response.plugins())
    {
        version.push_back(v);
    }
    return true;
}
bool HumanoidControllerClient::ClearError()
{
    ClientContext ctx;
    google::protobuf::Empty request;

    ErrorClearResponse response;
    Status status = m_system_stub_->ClearError(&ctx, request, &response);

    if (!status.ok())
    {
        std::cerr << "ClearError RPC failed: " << status.error_message() << '\n';
        return false;
    }
    std::cout << "ClearError success = " << std::boolalpha << response.success()
              << '\n';
    return true;
}
std::vector<std::tuple<uint32_t, double, double>> HumanoidControllerClient::getJointSoftLimit()
{
    ClientContext clientText;
    Empty request;
    GetJointParamsResponse response;
    std::vector<std::tuple<uint32_t, double, double>> result;
    Status status = m_params_stub_->GetJointSoftLimit(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetJointSoftLimit failed: " << status.error_message()
                  << std::endl;
    }
    else
    {
        auto jointParams = response.joint_params();
        for (auto jointParam : jointParams)
        {
            result.emplace_back(jointParam.joint_index(), jointParam.max_pos(), jointParam.min_pos());
            std::cout << "joint index:" << jointParam.joint_index()
                      << " max:" << jointParam.max_pos()
                      << " min:" << jointParam.min_pos() << std::endl;
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getJointMaxVel()
{
    ClientContext clientText;
    Empty request;
    GetJointParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status = m_params_stub_->GetJointMaxVel(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetJointMaxVel failed: " << status.error_message()
                  << std::endl;
    }
    else
    {
        auto jointParams = response.joint_params();
        for (auto jointParam : jointParams)
        {
            result.emplace_back(jointParam.joint_index(),
                                jointParam.max_vel());
            std::cout << "joint index:" << jointParam.joint_index()
                      << " max vel:" << jointParam.max_vel() << std::endl;
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getJointMaxAcc()
{
    ClientContext clientText;
    Empty request;
    GetJointParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status = m_params_stub_->GetJointMaxAcc(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetJointMaxAcc failed: " << status.error_message()
                  << std::endl;
    }
    else
    {
        auto jointParams = response.joint_params();
        for (auto jointParam : jointParams)
        {
            result.emplace_back(jointParam.joint_index(),
                                jointParam.max_acc());
            std::cout << "joint index:" << jointParam.joint_index()
                      << " max acc:" << jointParam.max_acc() << std::endl;
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getCartesianTranslationMaxVel()
{
    ClientContext clientText;
    Empty request;
    GetCartesianParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetCartesianTranslationMaxVel(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetCartesianTranslationMaxVel failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto cartParams = response.cartesian_params();
        for (auto cartParam : cartParams)
        {
            result.emplace_back(cartParam.cartesian_index(),
                                cartParam.trans_max_vel());
            std::cout << "cart index:" << cartParam.cartesian_index()
                      << " trans max vel:" << cartParam.trans_max_vel()
                      << std::endl;
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getCartesianTranslationMaxAcc()
{
    ClientContext clientText;
    Empty request;
    GetCartesianParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetCartesianTranslationMaxAcc(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetCartesianTranslationMaxAcc failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto cartParams = response.cartesian_params();
        for (auto cartParam : cartParams)
        {
            result.emplace_back(cartParam.cartesian_index(),
                                cartParam.trans_max_acc());
            std::cout << "cart index:" << cartParam.cartesian_index()
                      << " trans max acc" << cartParam.trans_max_acc() << std::endl;
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getCartesianRotationMaxVel()
{
    ClientContext clientText;
    Empty request;
    GetCartesianParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetCartesianRotationMaxVel(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetCartesianRotationMaxVel failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto cartParams = response.cartesian_params();
        for (auto cartParam : cartParams)
        {
            result.emplace_back(cartParam.cartesian_index(),
                                cartParam.rota_max_vel());
            std::cout << "cart index:" << cartParam.cartesian_index()
                      << " rota max vel:" << cartParam.rota_max_vel() << std::endl;
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getCartesianRotationMaxAcc()
{
    ClientContext clientText;
    Empty request;
    GetCartesianParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetCartesianRotationMaxAcc(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetCartesianRotationMaxAcc failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto cartParams = response.cartesian_params();
        for (auto cartParam : cartParams)
        {
            result.emplace_back(cartParam.cartesian_index(),
                                cartParam.rota_max_acc());
            std::cout << "cart Index:" << cartParam.cartesian_index()
                      << " rota max acc:" << cartParam.rota_max_acc() << std::endl;
        }
    }
    return result;
}

bool HumanoidControllerClient::setJointSoftLimit(int32_t jointcount, const std::vector<uint32_t> &joint_index, const std::vector<double> &maxPos, const std::vector<double> &minPos)
{
    ClientContext context;
    SetJointParamsRequest request;
    SetParamResponse response;

    if (jointcount <= 0)
    {
        std::cout << "Invalid joint count: " << jointcount << std::endl;
        return false;
    }
    if (joint_index.size() != jointcount ||
        maxPos.size() != jointcount ||
        minPos.size() != jointcount)
    {
        std::cout << "Parameter array size mismatch with joint count" << std::endl;
        return false;
    }
    for (int i = 0; i < jointcount; i++)
    {
        auto jointParam = request.add_joint_params();
        jointParam->set_joint_index(joint_index[i]);
        jointParam->set_max_pos(maxPos[i]);
        jointParam->set_min_pos(minPos[i]);
    }

    Status status = m_params_stub_->SetJointSoftLimit(&context, request, &response);
    if (status.ok())
    {
        std::cout << "RPC: SetJointSoftLimit call succeeded!" << std::endl;
        return response.success();
    }
    else
    {
        std::cout << "RPC: SetJointSoftLimit call failed: " << status.error_message() << std::endl;
        return false;
    }
}

bool HumanoidControllerClient::setJointMaxVel(int32_t jointcount, const std::vector<uint32_t> &joint_index, const std::vector<double> &maxVel)
{
    ClientContext context;
    SetJointParamsRequest request;
    SetParamResponse response;

    if (jointcount <= 0)
    {
        std::cout << "Invalid joint count: " << jointcount << std::endl;
        return false;
    }
    if (joint_index.size() != jointcount ||
        maxVel.size() != jointcount)
    {
        std::cout << "Parameter array size mismatch with joint count" << std::endl;
        return false;
    }
    for (int i = 0; i < jointcount; i++)
    {
        auto jointParam = request.add_joint_params();
        jointParam->set_joint_index(joint_index[i]);
        jointParam->set_max_vel(maxVel[i]);
    }

    Status status = m_params_stub_->SetJointMaxVel(&context, request, &response);
    if (status.ok())
    {
        std::cout << "RPC: SetJointMaxVel call succeeded!" << std::endl;
        return response.success();
    }
    else
    {
        std::cout << "RPC: SetJointMaxVel call failed: " << status.error_message() << std::endl;
        return false;
    }
}

bool HumanoidControllerClient::setJointMaxAcc(int32_t jointcount, const std::vector<uint32_t> &joint_index, const std::vector<double> &maxAcc)
{
    ClientContext context;
    SetJointParamsRequest request;
    SetParamResponse response;

    if (jointcount <= 0)
    {
        std::cout << "Invalid joint count: " << jointcount << std::endl;
        return false;
    }
    if (joint_index.size() != jointcount ||
        maxAcc.size() != jointcount)
    {
        std::cout << "Parameter array size mismatch with joint count" << std::endl;
        return false;
    }
    for (int i = 0; i < jointcount; i++)
    {
        auto jointParam = request.add_joint_params();
        jointParam->set_joint_index(joint_index[i]);
        jointParam->set_max_acc(maxAcc[i]);
    }

    Status status = m_params_stub_->SetJointMaxAcc(&context, request, &response);
    if (status.ok())
    {
        std::cout << "RPC: SetJointMaxAcc call succeeded!" << std::endl;
        std::cout << "Response: " << response.success() << std::endl;
        return response.success();
    }
    else
    {
        std::cout << "RPC: SetJointMaxAcc call failed!" << std::endl;
        std::cout << "Error: " << status.error_message() << std::endl;
        return false;
    }
}

bool HumanoidControllerClient::setCartesianTranslationMaxVel(int32_t jointcount, const std::vector<uint32_t> &cartesian_index, const std::vector<double> &transMaxVel)
{
    ClientContext context;
    SetCartesianParamsRequest request;
    SetParamResponse response;

    if (jointcount <= 0)
    {
        std::cout << "Invalid joint count: " << jointcount << std::endl;
        return false;
    }
    if (cartesian_index.size() != jointcount ||
        transMaxVel.size() != jointcount)
    {
        std::cout << "Parameter array size mismatch with joint count" << std::endl;
        return false;
    }
    for (int i = 0; i < jointcount; i++)
    {
        auto cartesianParam = request.add_cartesian_params();
        cartesianParam->set_cartesian_index(cartesian_index[i]);
        cartesianParam->set_trans_max_vel(transMaxVel[i]);
    }

    Status status = m_params_stub_->SetCartesianTranslationMaxVel(&context, request, &response);
    if (status.ok())
    {
        std::cout << "RPC: SetCartesianTranslationMaxVel call succeeded!"
                  << std::endl;
        std::cout << "Response: " << response.success() << std::endl;
        return response.success();
    }
    else
    {
        std::cout << "RPC: SetCartesianTranslationMaxVel call failed!"
                  << std::endl;
        std::cout << "Error: " << status.error_message() << std::endl;
        return false;
    }
}

bool HumanoidControllerClient::setCartesianTranslationMaxAcc(int32_t jointcount, const std::vector<uint32_t> &cartesian_index, const std::vector<double> &transMaxAcc)
{
    ClientContext context;
    SetCartesianParamsRequest request;
    SetParamResponse response;

    if (jointcount <= 0)
    {
        std::cout << "Invalid joint count: " << jointcount << std::endl;
        return false;
    }
    if (cartesian_index.size() != jointcount ||
        transMaxAcc.size() != jointcount)
    {
        std::cout << "Parameter array size mismatch with joint count" << std::endl;
        return false;
    }
    for (int i = 0; i < jointcount; i++)
    {
        auto cartesianParam = request.add_cartesian_params();
        cartesianParam->set_cartesian_index(cartesian_index[i]);
        cartesianParam->set_trans_max_acc(transMaxAcc[i]);
    }

    Status status = m_params_stub_->SetCartesianTranslationMaxAcc(&context, request, &response);
    if (status.ok())
    {
        std::cout << "RPC: SetCartesianTranslationMaxAcc call succeeded!"
                  << std::endl;
        std::cout << "Response: " << response.success() << std::endl;
        return response.success();
    }
    else
    {
        std::cout << "RPC: SetCartesianTranslationMaxAcc call failed!"
                  << std::endl;
        std::cout << "Error: " << status.error_message() << std::endl;
        return false;
    }
}

bool HumanoidControllerClient::setCartesianRotationMaxVel(int32_t jointcount, const std::vector<uint32_t> &cartesian_index, const std::vector<double> &rotaMaxVel)
{
    ClientContext context;
    SetCartesianParamsRequest request;
    SetParamResponse response;

    if (jointcount <= 0)
    {
        std::cout << "Invalid joint count: " << jointcount << std::endl;
        return false;
    }
    if (cartesian_index.size() != jointcount ||
        rotaMaxVel.size() != jointcount)
    {
        std::cout << "Parameter array size mismatch with joint count" << std::endl;
        return false;
    }
    for (int i = 0; i < jointcount; i++)
    {
        auto cartesianParam = request.add_cartesian_params();
        cartesianParam->set_cartesian_index(cartesian_index[i]);
        cartesianParam->set_rota_max_vel(rotaMaxVel[i]);
    }

    Status status = m_params_stub_->SetCartesianRotationMaxVel(&context, request, &response);
    if (status.ok())
    {
        std::cout << "RPC: SetCartesianRotationMaxVel call succeeded!"
                  << std::endl;
        std::cout << "Response: " << response.success() << std::endl;
        return response.success();
    }
    else
    {
        std::cout << "RPC: SetCartesianRotationMaxVel call failed!" << std::endl;
        std::cout << "Error: " << status.error_message() << std::endl;
        return false;
    }
}

bool HumanoidControllerClient::setCartesianRotationMaxAcc(int32_t jointcount, const std::vector<uint32_t> &cartesian_index, const std::vector<double> &rotaMaxAcc)
{
    ClientContext context;
    SetCartesianParamsRequest request;
    SetParamResponse response;

    if (jointcount <= 0)
    {
        std::cout << "Invalid joint count: " << jointcount << std::endl;
        return false;
    }
    if (cartesian_index.size() != jointcount ||
        rotaMaxAcc.size() != jointcount)
    {
        std::cout << "Parameter array size mismatch with joint count" << std::endl;
        return false;
    }
    for (int i = 0; i < jointcount; i++)
    {
        auto cartesianParam = request.add_cartesian_params();
        cartesianParam->set_cartesian_index(cartesian_index[i]);
        cartesianParam->set_rota_max_acc(rotaMaxAcc[i]);
    }

    Status status =
        m_params_stub_->SetCartesianRotationMaxAcc(&context, request, &response);
    if (status.ok())
    {
        std::cout << "RPC: SetCartesianRotationMaxAcc call succeeded!"
                  << std::endl;
        std::cout << "Response: " << response.success() << std::endl;
        return response.success();
    }
    else
    {
        std::cout << "RPC: SetCartesianRotationMaxAcc call failed!" << std::endl;
        std::cout << "Error: " << status.error_message() << std::endl;
        return false;
    }
}

std::vector<std::tuple<uint32_t, double, double>> HumanoidControllerClient::getJointMechanicalLimit()
{
    ClientContext clientText;
    Empty request;
    GetJointParamsResponse response;
    std::vector<std::tuple<uint32_t, double, double>> result;
    Status status =
        m_params_stub_->GetJointMechanicalLimit(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetJointMechanicalLimit failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto jointParams = response.joint_params();
        for (auto jointParam : jointParams)
        {
            result.emplace_back(jointParam.joint_index(), jointParam.max_pos(),
                                jointParam.min_pos());
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getJointMechanicalMaxVel()
{
    ClientContext clientText;
    Empty request;
    GetJointParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetJointMechanicalMaxVel(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetJointMechanicalMaxVel failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto jointParams = response.joint_params();
        for (auto jointParam : jointParams)
        {
            result.emplace_back(jointParam.joint_index(), jointParam.max_vel());
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getJointMechanicalMaxAcc()
{
    ClientContext clientText;
    Empty request;
    GetJointParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetJointMechanicalMaxAcc(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetJointMechanicalMaxAcc failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto jointParams = response.joint_params();
        for (auto jointParam : jointParams)
        {
            result.emplace_back(jointParam.joint_index(), jointParam.max_acc());
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getCartesianMechanicalTranslationMaxVel()
{
    ClientContext clientText;
    Empty request;
    GetCartesianParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetCartesianMechanicalTranslationMaxVel(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetCartesianMechanicalTranslationMaxVel failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto cartParams = response.cartesian_params();
        for (auto cartParam : cartParams)
        {
            result.emplace_back(cartParam.cartesian_index(),
                                cartParam.trans_max_vel());
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getCartesianMechanicalTranslationMaxAcc()
{
    ClientContext clientText;
    Empty request;
    GetCartesianParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetCartesianMechanicalTranslationMaxAcc(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetCartesianMechanicalTranslationMaxAcc failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto cartParams = response.cartesian_params();
        for (auto cartParam : cartParams)
        {
            result.emplace_back(cartParam.cartesian_index(),
                                cartParam.trans_max_acc());
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getCartesianMechanicalRotationMaxVel()
{
    ClientContext clientText;
    Empty request;
    GetCartesianParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetCartesianMechanicalRotationMaxVel(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetCartesianMechanicalRotationMaxVel failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto cartParams = response.cartesian_params();
        for (auto cartParam : cartParams)
        {
            result.emplace_back(cartParam.cartesian_index(),
                                cartParam.rota_max_vel());
        }
    }
    return result;
}

std::vector<std::pair<uint32_t, double>> HumanoidControllerClient::getCartesianMechanicalRotationMaxAcc()
{
    ClientContext clientText;
    Empty request;
    GetCartesianParamsResponse response;
    std::vector<std::pair<uint32_t, double>> result;
    Status status =
        m_params_stub_->GetCartesianMechanicalRotationMaxAcc(&clientText, request, &response);
    if (!status.ok())
    {
        std::cerr << "RPC: GetCartesianMechanicalRotationMaxAcc failed: "
                  << status.error_message() << std::endl;
    }
    else
    {
        auto cartParams = response.cartesian_params();
        for (auto cartParam : cartParams)
        {
            result.emplace_back(cartParam.cartesian_index(),
                                cartParam.rota_max_acc());
        }
    }
    return result;
}