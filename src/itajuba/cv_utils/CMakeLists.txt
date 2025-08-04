# Arquivos CMakeLists servem como passo-a-passos para o sistema de build CMake
# Em projetos ROS2, ele é fundamental para compilar o código, encontrar dependências e instalar arquivos necessários
#   nos locais corretos para que o ecossistema ROS possa encontrá-los

cmake_minimum_required(VERSION 3.5) # versão mínima do cmake
project(itajuba_cv_utils) # nome do projeto

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_python REQUIRED)
find_package(custom_msgs REQUIRED)
find_package(vision_msgs REQUIRED)

# Install launch files
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_target_dependencies(
  custom_msgs
)

ament_package() # finaliza o processamento do pacote.
