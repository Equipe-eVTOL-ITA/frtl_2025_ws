#ifndef DRONE_HPP_
#define DRONE_HPP_

#include <chrono>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <custom_msgs/msg/position.hpp>

#include <px4_msgs/msg/vehicle_status.hpp>
#include <px4_msgs/msg/vehicle_command.hpp>
#include <px4_msgs/msg/trajectory_setpoint.hpp>
#include <px4_msgs/msg/offboard_control_mode.hpp>
#include <px4_msgs/msg/timesync_status.hpp>
#include <px4_msgs/msg/vehicle_odometry.hpp>
#include <px4_msgs/msg/vehicle_rates_setpoint.hpp>
#include <px4_msgs/msg/airspeed.hpp>

#include <Eigen/Eigen>
#include <Eigen/Dense>

namespace DronePX4
{
enum ARMING_STATE
{
	DISARMED = 1,
	ARMED = 2
};

enum ARM_DISARM_REASON
{
  	TRANSITION_TO_STANDBY = 0,
  	RC_STICK = 1,
  	RC_SWITCH = 2,
  	COMMAND_INTERNAL = 3,
  	COMMAND_EXTERNAL = 4,
  	MISSION_START = 5,
  	SAFETY_BUTTON = 6,
  	AUTO_DISARM_LAND = 7,
  	AUTO_DISARM_PREFLIGHT = 8,
  	KILL_SWITCH = 9,
  	LOCKDOWN = 10,
  	FAILURE_DETECTOR = 11,
  	SHUTDOWN = 12,
  	ARM_DISARM_REASON_NONE = 13
};

enum FAILURE
{
	NONE = 0,
	ROLL = 1,
	PITCH = 2,
	ALT = 4,
	EXT = 8,
	ARM_ESC = 16,
	BATTERY = 32,
	IMBALANCED_PROP = 64,
	MOTOR = 128
};

enum FLIGHT_MODE
{
  	MANUAL = 0,               // Manual mode
  	ALTCTL = 1,               // Altitude control mode
  	POSCTL = 2,               // Position control mode
  	AUTO_MISSION = 3,         // Auto mission mode
  	AUTO_LOITER = 4,          // Auto loiter mode
  	AUTO_RTL = 5,             // Auto return to launch mode
  	ACRO = 6,                 // Acro mode
  	DESCEND = 7,              // Descend mode (no position control)
  	TERMINATION = 8,          // Termination mode
  	OFFBOARD = 9,             // Offboard
  	STAB = 10,                // Stabilized mode
  	AUTO_TAKEOFF = 11,        // Takeoff
  	AUTO_LAND = 12,           // Land
  	AUTO_FOLLOW_TARGET = 13,  // Auto Follow
  	AUTO_PRECLAND = 14,       // Precision land with landing target
  	ORBIT = 15,               // Orbit in a circle
  	AUTO_VTOL_TAKEOFF = 16,   // Takeoff, transition, establish loiter
  	UNKNOWN_MODE = 17
};

enum CONTROLLER_TYPE
{
  	NO_CONTROLLER = 0,        // No controller defined
  	POSITION = 1,             // Position control
  	VELOCITY = 2,             // Velocity control
  	BODY_RATES = 3,           // Body rates (rad/s) and thrust [-1, 1] controller
};
} // namespace DronePX4


class Drone : public rclcpp::Node
{
public:
	Drone();
	~Drone();


	/*
		Getters
	*/

	DronePX4::ARMING_STATE getArmingState();
	DronePX4::FLIGHT_MODE getFlightMode();
	DronePX4::ARM_DISARM_REASON getArmReason();
	DronePX4::ARM_DISARM_REASON getDisarmReason();
	DronePX4::FAILURE getFailure();

	Eigen::Vector3d getLocalPosition();

	float getAltitude();

	float getGroundSpeed();

	float getAirSpeed();

	Eigen::Vector3d getOrientation();

	/*
		Drone actions
	*/

	void arm();
	void armSync();

	void disarm();
	void disarmSync();

	//void takeoff();

	void land();

	void setLocalPosition(float x, float y, float z, float yaw);

	void setLocalPositionSync(
		double x,
		double y,
		double z,
		double yaw = std::numeric_limits<float>::quiet_NaN(),
		double airspeeed = 0.0,
		double distance_threshold = 0.1,
		DronePX4::CONTROLLER_TYPE controller_type = DronePX4::CONTROLLER_TYPE::POSITION);

	void setLocalVelocity(float vx, float vy, float vz, float yaw_rate = 0.0f);

	void setGroundSpeed(float speed);

	void setAirSpeed(float speed);

	void setOffboardControlMode(DronePX4::CONTROLLER_TYPE type);
	
	void toOffboardSync();

	void toPositionSync();

	void setHomePosition(const Eigen::Vector3d& fictual_home);
	
	void setOffboardMode();

	double getTime();

	void log(const std::string& info);


private:
	/// Send command to PX4
	/// \param[in] command Command ID
	/// \param[in] target_system System which should execute the command
	/// \param[in] target_component Component which should execute the command, 0 for all components
	/// \param[in] source_system System sending the command
	/// \param[in] source_component Component sending the command
	/// \param[in] confirmation 0: First transmission of this command
	/// 1-255: Confirmation transmissions
	/// \param[in] param1 Parameter 1, as defined by MAVLink uint16 VEHICLE_CMD enum.
	/// \param[in] param2 Parameter 2, as defined by MAVLink uint16 VEHICLE_CMD enum.
	/// \param[in] param3 Parameter 3, as defined by MAVLink uint16 VEHICLE_CMD enum.
	/// \param[in] param4 Parameter 4, as defined by MAVLink uint16 VEHICLE_CMD enum.
	/// \param[in] param5 Parameter 5, as defined by MAVLink uint16 VEHICLE_CMD enum.
	/// \param[in] param6 Parameter 6, as defined by MAVLink uint16 VEHICLE_CMD enum.
	/// \param[in] param7 Parameter 7, as defined by MAVLink uint16 VEHICLE_CMD enum.
	void sendCommand(
		uint32_t command, uint8_t target_system, uint8_t target_component, uint8_t source_system,
		uint8_t source_component, uint8_t confirmation, bool from_external,
		float param1 = 0.0f, float param2 = 0.0f, float param3 = 0.0f,
		float param4 = 0.0f, float param5 = 0.0f, float param6 = 0.0f,
		float param7 = 0.0f);

	/// Send a command to PX4 to set the speed
	/// \param[in] speed Speed to set in m/s
	/// \param[in] is_ground_speed True if the speed is a ground speed, false if it is an air speed
	void setSpeed(float speed, bool is_ground_speed);
	
	rclcpp::Subscription<px4_msgs::msg::VehicleStatus>::SharedPtr vehicle_status_sub_;
		
	rclcpp::Subscription<px4_msgs::msg::TimesyncStatus>::SharedPtr vehicle_timesync_sub_;
	
	rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr vehicle_odometry_sub_;
	
	rclcpp::Publisher<px4_msgs::msg::VehicleCommand>::SharedPtr vehicle_command_pub_;
		
	rclcpp::Publisher<px4_msgs::msg::TrajectorySetpoint>::SharedPtr vehicle_trajectory_setpoint_pub_;
	
	rclcpp::Publisher<px4_msgs::msg::OffboardControlMode>::SharedPtr vehicle_offboard_control_mode_pub_;
	
	rclcpp::Publisher<px4_msgs::msg::VehicleRatesSetpoint>::SharedPtr vehicle_rates_setpoint_pub_;
	
	rclcpp::Subscription<px4_msgs::msg::Airspeed>::SharedPtr vehicle_airspeed_sub_;

	rclcpp::Publisher<custom_msgs::msg::Position>::SharedPtr position_pub_;
	rclcpp::TimerBase::SharedPtr position_timer_;

	DronePX4::ARMING_STATE arming_state_{DronePX4::ARMING_STATE::DISARMED};
	DronePX4::FLIGHT_MODE flight_mode_{DronePX4::FLIGHT_MODE::UNKNOWN_MODE};
	DronePX4::ARM_DISARM_REASON arm_reason_;
	DronePX4::ARM_DISARM_REASON disarm_reason_;
	DronePX4::FAILURE failure_;

	int target_system_{1};

	float current_speed;
	std::chrono::time_point<std::chrono::high_resolution_clock> odom_timestamp_;
	float current_pos_x_;
	float current_pos_y_;
	float current_pos_z_;
	float current_vel_x_;
	float current_vel_y_;
	float current_vel_z_;
	float ground_speed_;
	float airspeed_{0};

	uint8_t target_component_{1};
	uint8_t source_system_{255};
	uint8_t source_component_{0};
	uint8_t confirmation_{1};
	bool from_external_{true};

	unsigned int vehicle_id_{0};

	std::chrono::time_point<std::chrono::high_resolution_clock> timestamp_;

	// save the most recent telemetry message data in these fields
	float roll_{0};
	float pitch_{0};
	float yaw_{0};

	Eigen::Vector3d frd_home_position_ = Eigen::Vector3d::Zero();
	Eigen::Vector3d ned_home_position_ = Eigen::Vector3d::Zero();
	float initial_yaw_{0};

    Eigen::Vector3d convertPositionNEDtoFRD(const Eigen::Vector3d& position_ned) const;
    Eigen::Vector3d convertPositionFRDtoNED(const Eigen::Vector3d& position_frd) const;
    Eigen::Vector3d convertVelocityNEDtoFRD(const Eigen::Vector3d& velocity_ned) const;
    Eigen::Vector3d convertVelocityFRDtoNED(const Eigen::Vector3d& velocity_frd) const;
};


#endif