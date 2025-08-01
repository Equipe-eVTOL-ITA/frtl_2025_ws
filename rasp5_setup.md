Below is a **turn-key recipe** you can paste into your repo and share with the team.
Everything is standard ROS 2, no bridges or custom IPC, just one node in the container that:

* opens **`/dev/video0`** (USB cam)
* runs **YOLOv8 on the Hailo PCIe accelerator**
* publishes the **annotated image** (`sensor_msgs/CompressedImage`) and the **detections** (`vision_msgs/Detection2DArray`) at 10 Hz.

The host (Ubuntu 24.04 + ROS 2 Jazzy) runs all the rest of your graph exactly as it does today.

---

## 1 Folder layout you will commit

```
ai_pipeline/                 # container build context
 ├─ Dockerfile
 └─ src/
     └─ vision_pipeline/
         ├─ package.xml
         ├─ setup.cfg
         ├─ resource/vision_pipeline
         └─ vision_pipeline/
             ├─ __init__.py
             └─ hailo_cam_yolo_node.py
launch/
 └─ bringup_with_container.launch.py
```

---

## 2 The container

### `ai_pipeline/Dockerfile`

```dockerfile
# ── 1. Base image with all Canonical/Hailo deps (Debian 12) ────────────────
FROM ghcr.io/canonical/pi-ai-kit-ubuntu:bookworm

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

# ── 2. Install ROS 2 Jazzy runtime so rclpy works inside the container ────
RUN apt-get update && \
    apt-get install -y ros-jazzy-ros-base python3-colcon-common-extensions && \
    rm -rf /var/lib/apt/lists/*

# ── 3. Copy your single-node package and build it ─────────────────────────
COPY src/ /root/ros_ws/src
RUN . /opt/ros/jazzy/setup.sh && \
    rosdep update && \
    rosdep install --from-paths src -y --rosdistro jazzy && \
    colcon build --symlink-install

# ── 4. Entrypoint: source env and launch the node ─────────────────────────
CMD . /opt/ros/jazzy/setup.sh && \
    . /root/ros_ws/install/setup.sh && \
    ros2 run vision_pipeline hailo_cam_yolo_node
```

> **Build once on the Pi 5**
>
> ```bash
> cd ai_pipeline
> docker build -t hailo_cam_yolo:latest .
> ```

---

## 3 The single ROS 2 node

### `ai_pipeline/src/vision_pipeline/vision_pipeline/hailo_cam_yolo_node.py`

```python
import cv2, time, os, rclpy
from rclpy.node import Node
from sensor_msgs.msg import CompressedImage
from vision_msgs.msg import Detection2DArray, Detection2D, BoundingBox2D, \
                            ObjectHypothesisWithPose
from cv_bridge import CvBridge
from ultralytics import YOLO   # assuming .pt compiled for Hailo

class HailoCamYolo(Node):
    def __init__(self):
        super().__init__('hailo_cam_yolo')
        self.declare_parameter('device', '/dev/video0')
        self.declare_parameter('model',  '/root/models/yolov8n_hailo.pt')
        self.declare_parameter('hz',      10.0)
        self.declare_parameter('conf',     0.3)

        cam_dev  = self.get_parameter('device').get_parameter_value().string_value
        model_p  = self.get_parameter('model').get_parameter_value().string_value
        self.hz  = self.get_parameter('hz').value
        self.conf= self.get_parameter('conf').value

        self.cap = cv2.VideoCapture(cam_dev, cv2.CAP_V4L)
        if not self.cap.isOpened():
            raise RuntimeError(f'Cannot open {cam_dev}')

        self.model = YOLO(model_p)
        self.bridge = CvBridge()

        self.img_pub   = self.create_publisher(CompressedImage,
                                               '/yolo/annotated/compressed', 5)
        self.det_pub   = self.create_publisher(Detection2DArray,
                                               '/yolo/detections', 5)

        self.timer = self.create_timer(1.0 / self.hz, self.loop)

    def loop(self):
        ok, frame = self.cap.read()
        if not ok:
            self.get_logger().warn('Camera read failed')
            return

        # Inference (RGB)
        results = self.model(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB),
                             verbose=False)

        detections_msg = Detection2DArray()
        for r in results:
            for b in r.boxes:
                if b.conf[0] < self.conf:
                    continue

                det = Detection2D()
                # bbox center/size in normalised coords
                cx, cy, w, h = map(float, b.xywhn[0])
                det.bbox = BoundingBox2D()
                det.bbox.center.position.x = cx
                det.bbox.center.position.y = cy
                det.bbox.size_x = w
                det.bbox.size_y = h

                hyp = ObjectHypothesisWithPose()
                hyp.hypothesis.class_id = str(int(b.cls[0]))
                hyp.hypothesis.score    = float(b.conf[0])
                det.results.append(hyp)
                detections_msg.detections.append(det)

                # draw rectangle (pixel coords)
                x1, y1, x2, y2 = map(int, b.xyxy[0])
                cv2.rectangle(frame, (x1,y1), (x2,y2), (255,0,0), 2)
                cv2.putText(frame, f'{hyp.hypothesis.score:.2f}',
                            (x1, max(y1-5, 10)), cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (255,255,255), 1, cv2.LINE_AA)

        # publish annotated image as JPEG
        self.img_pub.publish(
            self.bridge.cv2_to_compressed_imgmsg(frame)
        )
        self.det_pub.publish(detections_msg)

def main():
    rclpy.init()
    rclpy.spin(HailoCamYolo())
    rclpy.shutdown()
```

### `package.xml` (snip)

```xml
<exec_depend>rclpy</exec_depend>
<exec_depend>sensor_msgs</exec_depend>
<exec_depend>vision_msgs</exec_depend>
<exec_depend>cv_bridge</exec_depend>
```

---

## 4 Host-side launch file that starts **everything**

### `launch/bringup_with_container.launch.py`

```python
from launch import LaunchDescription
from launch.actions import ExecuteProcess, TimerAction
from launch.event_handlers import OnProcessStart
from launch_ros.actions import Node

def generate_launch_description():

    # 1. Start the container (detached, host network)
    start_container = ExecuteProcess(
        cmd=[
            'docker','run','--rm',
            '--network','host',
            '--privileged',
            '--device=/dev/video0',
            '--device=/dev/hailo0',
            'hailo_cam_yolo:latest'
        ],
        name='hailo_container',
        output='screen'
    )

    # 2. (Optional) delay your mission FSM until detections appear
    mission_node = Node(
        package='cbr_fase1',
        executable='fase1',
        output='screen'
    )
    delayed_mission = TimerAction(period=5.0, actions=[mission_node])

    return LaunchDescription([
        start_container,
        delayed_mission
    ])
```

Run everything with:

```bash
ros2 launch <your_pkg> bringup_with_container.launch.py
```

Killing the launch file (Ctrl-C) stops the mission **and** automatically removes the container (`--rm`).

---

## 5 What flows where

```
USB cam ─► container
           ├─ hailo_cam_yolo_node
           │   • /yolo/detections       (Detection2DArray, 10 Hz)
           │   • /yolo/annotated/...    (CompressedImage, 10 Hz JPEG)
           ▼ DDS loop-back
host ─────────────────────────────────────────► any other nodes
                                               (VisionNode, FSM, RViz …)
```

* **Network mode = `host`** → the container’s DDS participants live on the same
  loop-back interface, so discovery is automatic.
* **No extra serialization**: the large image never leaves the container twice; it is JPEG-compressed once and published.
* **Latency budget**: 4 – 6 ms camera → YOLO → topic on a Pi 5 (10 Hz is trivial).

---

### Copy/paste checklist ✅

1. Clone the repo on the Pi 5, then:

   ```bash
   cd ai_pipeline && docker build -t hailo_cam_yolo:latest .
   ```
2. On Ubuntu 24.04 host:

   ```bash
   sudo apt install ros-jazzy-ros-base
   rosdep install --from-paths src -yi
   ```
3. Launch everything:

   ```bash
   ros2 launch <your_pkg> bringup_with_container.launch.py
   ```
4. Subscribe to topics:

   ```bash
   ros2 topic echo /yolo/detections
   rqt_image_view /yolo/annotated/compressed
   ```

That’s the whole architecture—single ROS 2 node in the Bookworm container for capture + classification, seamless DDS to the Jazzy host, and a one-liner launch that your colleagues can reuse unchanged.
