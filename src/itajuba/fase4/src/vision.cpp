#include <rclcpp/rclcpp.hpp>
#include <vision_msgs/msg/detection2_d_array.hpp>
#include <std_msgs/msg/float64.hpp>
#include <math.h>

using namespace std_msgs::msg;

typedef vision_msgs::msg::Detection2DArray Detections;
typedef std::chrono::steady_clock Tempo;

struct BoundingBox {
    float cx, cy;
    float w, h;
    float confidence;
    std::string class_id;
    int64_t timestamp;
};

class VisionNode : public rclcpp::Node {
public:
    VisionNode() : Node("fase4_vision") {
        rclcpp::QoS vision_qos(10);
        vision_qos.best_effort();
        vision_qos.durability(rclcpp::DurabilityPolicy::Volatile);

        this->checkpoint_sub_ = this->create_subscription<Detections>(
            "/vertical_camera/classification",
            vision_qos,
            std::bind(&VisionNode::checkpointCallback, this, std::placeholders::_1)
        );

        this->lane_angle_sub_ = this->create_subscription<Float64>(
            "/lane_direction/vector2d",
            vision_qos,
            std::bind(&VisionNode::laneCallback, this, std::placeholders::_1)
        );
    }

    float getAngle(){
        return this->angle_;
    }

private:
    rclcpp::Subscription<Float64>::SharedPtr lane_angle_sub_;
    rclcpp::Subscription<Detections>::SharedPtr checkpoint_sub_;
    
    Tempo::time_point checkpoint_last_update_;
    Tempo::time_point lane_angle_last_update_;

    // Armazenamento de dados
    std::vector<BoundingBox> checkpoints_;
    float angle_ = 0.0;

    // Funções de Callback

    // Função para dar um update no vetor de bboxes
    void checkpointCallback(const Detections::SharedPtr msg){
        this->checkpoints_.clear(); // remove todos as bboxes detectadas da ultima vez

        for(const auto& detection : msg->detections){
            if(!detection.results.empty()){
                BoundingBox bbox;
                bbox.cx = detection.bbox.center.position.x;
                bbox.cy = detection.bbox.center.position.y;
                bbox.w = detection.bbox.size_x;
                bbox.h = detection.bbox.size_y;
                bbox.confidence = detection.results[0].hypothesis.score;
                bbox.class_id = detection.results[0].hypothesis.class_id;
                bbox.timestamp = msg->header.stamp.sec * 1000000000LL + msg->header.stamp.nanosec;
            
                this->checkpoints_.push_back(bbox);
            }
        }
        this->checkpoint_last_update_ = Tempo::now();
    }

    void laneCallback(const Float64::SharedPtr theta) {
        auto angle = Float64();

        if(angle.data >= M_PI)
            angle.data -= M_PI;
        
        angle.data = M_PI_2 - angle.data; // Angulo entre a vertical e a direção da figura

        this->angle_ = angle.data;
    }
};