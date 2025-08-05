#include "fsm/fsm.hpp"
#include <rclcpp/rclcpp.hpp>
#include "drone/Drone.hpp"

#include "TakeoffState.hpp"

using namespace fsm;

typedef Eigen::Vector3d V3;
typedef std::map<std::string, std::variant<double, std::string>> Params;

class Fase4FSM : public FSM {
public:
    Fase4FSM(
        std::shared_ptr<Drone> drone,
        const Params& params
    ) : fsm::FSM({"ERROR", "FINISHED"}){

        this->blackboard_set<std::shared_ptr<Drone>>("drone", drone);

        const V3 orientation = drone->getOrientation();

        // atualiza a blackboard com dados do tipo numerico ou string
        for(const auto& [key, value] : params){
            if(std::holds_alternative<double>(value))
                this->blackboard_set<float>(key, static_cast<float>(std::get<double>(value)));
            else if(std::holds_alternative<std::string>(value))
                this->blackboard_set<std::string>(key, std::get<std::string>(value));
        }

        this->blackboard_set<float>("initial_yaw", orientation[2]);
        float takeoff_height = std::get<double>(params.at("takeoff_height"));
    
        this->add_state("TAKEOFF", std::make_unique<TakeoffState>());
        
        this->add_transitions("TAKEOFF", {
            {"INITIAL TAKEOFF COMPLETED", "FINISHED"},
            {"SEG FAULT", "ERROR"}
        });

        /*
        this->add_transitions("estado de partida", {
            {"output do estado", "novo estado"},
            {"outro output do estado", "outro novo estado"}
        });
        */
    }
};

class NodeFSM : public rclcpp::Node {
public:
    NodeFSM(std::shared_ptr<Drone> drone) : rclcpp::Node("fase4_fsm") {
        this->drone_ptr_ = drone;
        
        // valores padrao (podem ser sobrescritos pelo .yaml)
        Params default_params = {
            {"takeoff_height", -2.0},
            {"max_vertical_velocity", 2.0},
            {"max_height_error", 0.15}
        };
        
        auto params = this->setupParams(default_params);

        this->fsm_ = std::make_unique<Fase4FSM>(this->drone_ptr_, params);

        /*
        Um wall timer permite que um nó execute funções em intervalos de tempo reais e regulares
        
        Wall Clock Timer se refere ao relógio REAL do seu computador
        */
        this->timer_ = this->create_wall_timer(
            std::chrono::milliseconds(50),
            std::bind(&NodeFSM::executeFSM, this) // Esse objeto de função (functor), quando chamado, executará o método de classe "executeFSM" do objeto this.
        );
    }

    void executeFSM(){
        if(rclcpp::ok() && !this->fsm_->is_finished())
            this->fsm_->execute();
        else
            rclcpp::shutdown();
    }

private:
    std::shared_ptr<Drone> drone_ptr_;
    std::shared_ptr<Fase4FSM> fsm_;
    rclcpp::TimerBase::SharedPtr timer_;


    Params setupParams(const Params& defaults){
        Params result;

        for(const auto& [name, default_value] : defaults){
            if(std::holds_alternative<double>(default_value)){
                this->declare_parameter(name, std::get<double>(default_value));
                result[name] = this->get_parameter(name).as_double();
            }
            else if(std::holds_alternative<std::string>(default_value)) {
                this->declare_parameter(name, std::get<std::string>(default_value));
                result[name] = this->get_parameter(name).as_string();
            }
        }

        return result;
    }
};

int main(int argc, const char *argv[]){
    rclcpp::init(argc, argv);
    rclcpp::executors::MultiThreadedExecutor executor;

    auto drone = std::make_shared<Drone>();
    auto fsm_node = std::make_shared<NodeFSM>(drone);

    executor.add_node(drone);
    executor.add_node(fsm_node);

    executor.spin();

    rclcpp::shutdown();
    return 0;
}