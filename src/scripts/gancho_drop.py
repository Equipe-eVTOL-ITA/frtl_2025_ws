# claw_servo.py   – place somewhere in $PATH or give full path
import RPi.GPIO as GPIO, sys, time

PIN = 33                     # physical pin you already use
OPEN_DUTY  = 3.0             # %
CLOSE_DUTY = 12.0            # %
ACTION = sys.argv[1] if len(sys.argv) > 1 else "open"   # default

GPIO.setmode(GPIO.BOARD)
GPIO.setup(PIN, GPIO.OUT, initial=GPIO.LOW)
pwm = GPIO.PWM(PIN, 50)      # 50 Hz
pwm.start(0)

try:
    duty = OPEN_DUTY if ACTION == "open" else CLOSE_DUTY
    pwm.ChangeDutyCycle(duty)
    time.sleep(0.5)
finally:
    pwm.ChangeDutyCycle(0)
    pwm.stop()
    GPIO.cleanup()
