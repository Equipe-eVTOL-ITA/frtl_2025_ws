cmake_minimum_required(VERSION 3.8)
project(itajuba_fase4)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#find dependencies
find_package(ament_cmake REQUIRED)
find_package(fsm REQUIRED)
find_package(itajuba_drone_lib REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(rclcpp REQUIRED)
find_package(OpenCV 4 REQUIRED)
find_package(vision_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)

include_directories(include)
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/../fsm/fsm/include)

#fase1

add_executable(fase4
  src/fase4.cpp
)

ament_target_dependencies(fase4
  rclcpp
  fsm
  itajuba_drone_lib
  Eigen3
  vision_msgs
  sensor_msgs
  cv_bridge
)

target_link_libraries(fase4 ${OpenCV_LIBS})

#Install

install(
  TARGETS
    fase4
  DESTINATION lib/${PROJECT_NAME}
)

# Install launch files
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
