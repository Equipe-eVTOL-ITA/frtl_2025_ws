#!/usr/bin/env python3
import argparse, yaml, depthai as dai, os, tempfile

def make_yaml(width:int, height:int, out_path:str):
    dev = dai.Device()                          # open any OAK connected
    calib = dev.readCalibration()

    K_L = calib.getCameraIntrinsics(dai.CameraBoardSocket.LEFT , width, height)
    K_R = calib.getCameraIntrinsics(dai.CameraBoardSocket.RIGHT, width, height)
    D_L = calib.getDistortionCoefficients(dai.CameraBoardSocket.LEFT)
    D_R = calib.getDistortionCoefficients(dai.CameraBoardSocket.RIGHT)
    baseline = calib.getBaselineDistance(dai.CameraBoardSocket.RIGHT,
                                         dai.CameraBoardSocket.LEFT) / 100.0

    data = {
      "Camera.type"  : "Stereo",
      "Camera.width" : width,
      "Camera.height": height,
      "Camera.fps"   : 30,

      # left intrinsics / dist
      "Camera.fx": K_L[0][0],  "Camera.fy": K_L[1][1],
      "Camera.cx": K_L[0][2],  "Camera.cy": K_L[1][2],
      "Camera.k1": D_L[0],     "Camera.k2": D_L[1],
      "Camera.p1": D_L[2],     "Camera.p2": D_L[3],
      "Camera.k3": D_L[4],

      # right intrinsics / dist
      "Camera2.fx": K_R[0][0], "Camera2.fy": K_R[1][1],
      "Camera2.cx": K_R[0][2], "Camera2.cy": K_R[1][2],
      "Camera2.k1": D_R[0],    "Camera2.k2": D_R[1],
      "Camera2.p1": D_R[2],    "Camera2.p2": D_R[3],
      "Camera2.k3": D_R[4],

      "Stereo.b": baseline,

      # nominal IMU noise (ORB-SLAM3 ref. values)
      "IMU.NoiseGyro": 1.7e-4,  "IMU.NoiseAcc": 2.0e-3,
      "IMU.GyroWalk": 1.7e-5,  "IMU.AccWalk": 2.0e-4
    }
    with open(out_path, "w") as f: yaml.dump(data, f, default_flow_style=False)
    print("Wrote ORB settings to", out_path)

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--w", type=int, default=640)
    ap.add_argument("--h", type=int, default=400)
    ap.add_argument("--out", default=os.path.join(tempfile.gettempdir(),"oak_stereo.yaml"))
    args = ap.parse_args()
    make_yaml(args.w, args.h, args.out)
