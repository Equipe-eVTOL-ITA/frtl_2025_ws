import rclpy
from rclpy.node import Node
from std_msgs.msg import UInt8
from sensor_msgs.msg import CompressedImage
from geometry_msgs.msg import Point

import cv2
import numpy as np
import math
from cv_bridge import CvBridge, CvBridgeError

class LaneDetector(Node):
	def __init__(self):
		super().__init__('lane_detector')

		self._centroid_publisher = self.create_publisher(Point, 'centroid', 10)
		self._threshold_publisher = self.create_publisher(UInt8, 'threshold', 10)
		self._subscriber = self.create_subscription(
			CompressedImage,
			'camera/compressed',
			self.lane_detection_callback,
			10
		)
		self.bridge = CvBridge()

		self.threshold: UInt8 = UInt8()
		self.threshold.data = int(8) # valor do threshold
	
	def lane_detection_callback(self, msg):
		np_arr = np.frombuffer(msg.data, np.uint8)
		cv_image = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)

		if cv_image is None:
			self.get_logger().error('Failed to decode compressed color image.')
			return

		hsv_image = cv2.cvtColor(cv_image, cv2.COLOR_BGR2HSV)

		lower_blue = np.array([100, 150, 50])
		upper_blue = np.array([140, 255, 255])

		binary_mask = cv2.inRange(hsv_image, lower_blue, upper_blue)

		contornos, hierarquia = cv2.findContours(
            binary_mask,
            cv2.RETR_TREE,
            cv2.CHAIN_APPROX_SIMPLE
        )

		output_image = cv_image.copy()

		if contornos:
			# filtra por tamanho (a faixa deve ser o maior contorno na imagem)
			maior_contorno = max(contornos, key=cv2.contourArea)
			area = cv2.contourArea(maior_contorno)

			if area > 200:
				cv2.drawContours(output_image, [maior_contorno], 0, (0, 255, 0), 3)

				# técnica dos momentos de imagem
				M = cv2.moments(maior_contorno)
				A = M['m00']
				if A > 0:
					# centroide
					cx = int(M['m10']/A)
					cy = int(M['m01']/A)
					cv2.circle(output_image, (cx, cy), 7, (0, 0, 255), -1)

					# orientacao
					mu20 = M['mu20']
					mu02 = M['mu02']
					mu11 = M['mu11']
					theta = 0.5*math.atan2(2*mu11, mu20 - mu02)

					# desenha a seta da orientação
					tamanho = 100
					start_point = (cx, cy)
					end_point = (
						int(cx + tamanho*math.cos(theta)),
						int(cy + tamanho*math.sin(theta))
					)
					cv2.arrowedLine(output_image, start_point, end_point, (0, 0, 255), 3)

		#cv2.imshow('mascara', binary_mask)
		cv2.imshow('Lane Detection', output_image)
		cv2.waitKey(1)



def main(args=None):
	rclpy.init(args=args)
	ld = LaneDetector()
	rclpy.spin(ld)
	ld.destroy_node()
	rclpy.shutdown()

if __name__ == '__main__':
	main()