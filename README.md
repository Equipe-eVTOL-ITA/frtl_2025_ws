# Workspace da FRTL 2025

Vamos fazer o setup dos pacotes necessários para enviar comandos de offboard control para o drone no Gazebo utilizando PX4-Autopilot, ROS 2 e o protocolo uXRCE-DDS.

### Explicando o que é cada pacote

- **PX4-Autopilot**: é o mesmo firmware de PX4 1.15 que roda na PixHawk. Por default, esse pacote também baixa o Gazebo.

- **ROS 2**: Sistema operacional que facilita a comunicação entre diferentes partes do drone, tanto entre componentes físicos como de software.

- **MicroXRCE-Agent**: Agente que traduz tópicos de ROS 2 para tópicos uORB (PX4).

- **px4_msgs**: Mensagens ROS 2 utilizadas pelo PX4.

- **frtl_2025**: contém a classe Drone (implementa funções que enviam px4_msgs) e também as máquinas de estados para resolver as fases.

- **camera_publisher**: pacote simples que publica tópicos de Imagens de ROS 2 a partir do feed de vídeo do dispositivo (webcam ou câmera)

- **gesture_classifier**: rede neural que recebe imagens e publica os gestos de mão classificados.

- **custom_msgs**: conjunto de mensagens personalizadas de ROS 2 para a eVTOL.



### Explicando a estrutura do workspace

```
home/
├── PX4-Autopilot
├── MicroXRCE-Agent
├── frtl_2024_ws/
    ├── src/
        ├── px4_msgs
        ├── px4_ros_com
        ├── frtl_2024
        ├── simulation
    ├── tasks/
        ├── build_ws.sh
        ├── simulate.sh
```

## Pré-requisitos

- **Sistema Operacional**: Ubuntu 22.04
- **Dependências**:
  - Github CLI (Configure aqui)
  - CMake
  - Python 3
  - Pip

### Configurando o GitHub

- Baixando gh:
    ```bash
    (type -p wget >/dev/null || (sudo apt update && sudo apt-get install wget -y)) \
    && sudo mkdir -p -m 755 /etc/apt/keyrings \
    && wget -qO- https://cli.github.com/packages/githubcli-archive-keyring.gpg | sudo tee /etc/apt/keyrings/githubcli-archive-keyring.gpg > /dev/null \
    && sudo chmod go+r /etc/apt/keyrings/githubcli-archive-keyring.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | sudo tee /etc/apt/sources.list.d/github-cli.list > /dev/null \
    && sudo apt update \
    && sudo apt install gh -y
    ```
- gh auth login: rode `gh auth login`, depois siga os passos selecionando HTTPS. 

- Obs: certifique-se de que você tem acesso ao repositório da eVTOL no Github.


## Setup do Ambiente

### 1. Instalar Dependências

```sh
sudo apt update
sudo apt install -y git cmake python3-colcon-common-extensions
pip install --user -U empy==3.3.4 pyros-genmsg setuptools==59.6.0
```

### 2. Instalar QGroundControl

1. Rode no terminal:
```bash
sudo usermod -a -G dialout $USER
sudo apt-get remove modemmanager -y
sudo apt install gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-gl -y
sudo apt install libfuse2 -y
sudo apt install libxcb-xinerama0 libxkbcommon-x11-0 libxcb-cursor0 -y
```

2. **Faça logout e login** para valer as mudanças.

3.  Download o [QGroundControl.AppImage](https://d176tv9ibo4jno.cloudfront.net/latest/QGroundControl.AppImage)

4. Rode no terminal, na pasta em que está o download (`cd ~/Downloads`):
    ```bash
    chmod +x ./QGroundControl.AppImage
    ./QGroundControl.AppImage
    ```

### 3. Instalar ROS 2 Humble

```sh
sudo apt update && sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8
sudo apt install software-properties-common
sudo add-apt-repository universe
sudo apt update && sudo apt install curl -y
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null
sudo apt update && sudo apt upgrade -y
sudo apt install ros-humble-desktop
sudo apt install ros-dev-tools
source /opt/ros/humble/setup.bash && echo "source /opt/ros/humble/setup.bash" >> .bashrc
```

### 4. Baixar MicroXRCE-Agent

```sh
cd ~
git clone https://github.com/eProsima/Micro-XRCE-DDS-Agent.git
cd Micro-XRCE-DDS-Agent
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig /usr/local/lib/
```

### 5. Baixar PX4-Autopilot (com Gazebo)

```sh
cd ~
git clone --branch v1.15.4 --recursive --depth 1 https://github.com/PX4/PX4-Autopilot.git
cd PX4-Autopilot/Tools/simulation/gz
git checkout main
git remote remove origin
git remote add origin https://github.com/Equipe-eVTOL-ITA/PX4-gazebo-models.git
git pull origin main --rebase
cd ~
bash ./PX4-Autopilot/Tools/setup/ubuntu.sh
```

Faça logout e login de novo para efetivar as mudanças.

```sh
cd ~/PX4-Autopilot
make px4_sitl 
```

OBS: Quando perguntado sobre `IF YOU DID NOT CHANGE THIS FILE (OR YOU DON'T KNOW WHAT A SUBMODULE IS):`, Escreva `y` e pressione `ENTER`.

### 6. Setup do frtl_2025_ws

- Primeiramente, clone o repositório `frtl_2025_ws`:

  ```bash
  cd ~
  git clone https://github.com/Equipe-eVTOL-ITA/frtl_2025_ws.git
  cd frtl_2025_ws
  code .
  ```
O repositório deve abrir no VSCode.

 - Execute a task Setup (`crtl+shift+P` e selecione `Tasks: run Task`).

O script `setup.sh` fará o download dos repositórios necessários, além de buildar o workspace todo.

### OBS: Memória RAM insuficiente para build

Se o seu PC congelar durante o processo de build no setup.sh, pode ser que você esteja com memória RAM insuficiente (geralmente 16GB não é suficiente). Para resolver, vamos aumentar o tamanho da memória Swap (espaço de armazenamento tratado como RAM).

```bash
# Turn swap off
# This moves stuff in swap to the main memory and might take several minutes
sudo swapoff -a

# Create an empty swapfile
# Note that "1M" is basically just the unit and count is an integer.
# Together, they define the size. In this case 8GiB.
sudo dd if=/dev/zero of=/swapfile bs=1M count=8192

# Set the correct permissions
sudo chmod 0600 /swapfile

sudo mkswap /swapfile  # Set up a Linux swap area
sudo swapon /swapfile  # Turn the swap on
```

Depois, rode: 

`sudo nano /etc/fstab` 

E cole na última linha:

 `/swapfile none swap sw 0 0`


## Teste para ver se está tudo ok

1. **Execute a task `simulate & image bridge & agent`. Selecione o mundo `fase1`.**

Isso deve abrir a simulação (Gazebo + PX4) no primeiro terminal, o image_bridge no segundo terminal e o agente MicroXRCE-DDS no terceiro terminal.

2. **Teste um dos nós de ROS 2 para controle offboard do drone:**

  Abrindo um novo terminal (quarto terminal), execute:

```sh
source install/setup.bash
ros2 run cbr_2025_fase1 fase1_dummy
```

O drone deve executar um circuito básico de landing e takeoff nas bases da arena.

## Rodando a simulação

- A task `simulate & image bridge & agent` faz, como o nome diz, 3 coisas em uma.

- Para rodar a simulação com controle Offboard, `simulate` e `agent` sempre serão necessários.

- Para simulações em que a câmera não é necessária, é recomendável rodar somente as tasks `simulate` e `agent`. Assim, o computador não precisa gastar recursos rodando o `image_bridge`.

- Para rodar os nós de offboard control, basta executar:
  ```bash
  #Sempre importar o ros2 para o terminal
  source /opt/ros/humble/setup.bash && source install/local_setup.bash

  #Rodar o no de ros2
  ros2 run <nome_do_pacote> <nome_do_executavel>
  ```
    - Ex: `ros2 run frtl_2024_fase1 fase1`

## Yolo Classifier

A classe YOLO é importada do Ultralytics. Se você quiser rodar a classificação por Yolo, você vai precisar baixar os seguintes pacotes:

```bash
pip uninstall numpy && pip install numpy==1.26.4
pip install ultralytics
```

Depois disso, basta:

```bash
ros2 run cbr_2025_cv_utils yolo_classifier
```


## Referências

- [PX4 ROS 2 User Guide](https://docs.px4.io/main/en/ros2/user_guide.html#installation-setup)
- [ROS 2 Offboard Control Example](https://docs.px4.io/main/en/ros2/offboard_control.html)

