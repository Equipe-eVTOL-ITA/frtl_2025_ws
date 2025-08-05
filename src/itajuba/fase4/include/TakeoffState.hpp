#include <Eigen/Eigen>
#include <opencv2/highgui.hpp>
#include "fsm/fsm.hpp"
#include "drone/Drone.hpp"

typedef fsm::Blackboard BB;
typedef Eigen::Vector3d V3;

class TakeoffState : public fsm::State {
public:
    TakeoffState() : fsm::State() {}

    void on_enter(BB &blackboard) override {
        this->drone = *blackboard.get<std::shared_ptr<Drone>>("drone");
        if(this->drone == nullptr) return;

        this->drone->log("STATE: TAKEOFF");

        const V3 home = V3({0.0, 0.0, 0.0});

        float takeoff_height    = *blackboard.get<float>("takeoff_height");
        this->max_velocity      = *blackboard.get<float>("max_vertical_velocity");
        this->max_height_error  = *blackboard.get<float>("max_height_error");

        this->drone->toOffboardSync();
        this->drone->armSync();
        this->drone->setHomePosition(home);

        this->pos = this->drone->getLocalPosition();
        this->initial_yaw = this->drone->getOrientation()[2];
        this->goal = V3({
            pos[0], pos[1], takeoff_height
        });

        this->drone->log("Initial Yaw: " + std::to_string(this->initial_yaw));
        this->drone->log("Home at: " + 
                            std::to_string(pos[0]) + " " +
                            std::to_string(pos[1]) + " " + 
                            std::to_string(pos[2])
                        );
    }

    std::string act(BB &blackboard) override {
        (void)blackboard; // evita warnings de compilação

        this->pos = this->drone->getLocalPosition();

        if((this->pos - this->goal).norm() < this->max_height_error)
            return "INITIAL TAKEOFF COMPLETED";

        this->delta_pos = this->goal - this->pos;
        this->little_goal = this->pos + 
                            (this->delta_pos.norm() > this->max_velocity ? this->delta_pos.normalized()*max_velocity : this->delta_pos);
        
        this->drone->setLocalPosition(
            this->little_goal[0],
            this->little_goal[1],
            this->little_goal[2],
            this->initial_yaw
        );

        return "";
    }

private:
    float initial_yaw, max_velocity, max_height_error;
    std::shared_ptr<Drone> drone;
    V3 pos, goal, delta_pos, little_goal;
};