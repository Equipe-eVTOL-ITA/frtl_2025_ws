#include "fsm/fsm.hpp"
#include "drone/Drone.hpp"
#include <Eigen/Eigen>

typedef fsm::Blackboard BB;
typedef Eigen::Vector3d V3;

class SearchLaneState : public fsm::State {
public:
    SearchLaneState() : fsm::State() {}

    void on_enter(BB &bb) override {
        this->drone = *bb.get<std::shared_ptr<Drone>>("drone");
        this->vision = *bb.get<std::shared_ptr<VisionNode>>("vision");
        if(this->drone == nullptr || this->vision == nullptr) return;

        this->drone->log("STATE: Searching Lane...");

        this->initial_yaw = *bb.get<float>("initial_yaw");
    }

    std::string act(BB &bb) override {
        (void)bb;

        float angle = this->vision->getAngle();

        return "";
    }

private:
    float initial_yaw, max_velocity, angle;
    std::shared_ptr<Drone> drone;
    std::shared_ptr<VisionNode> vision;
    V3 pos;
};