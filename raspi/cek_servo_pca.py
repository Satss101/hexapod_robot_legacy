from time import sleep
from adafruit_servokit import ServoKit

# Set channels to the number of servo channels on your kit.
# 8 for FeatherWing, 16 for Shield/HAT/Bonnet.
kit = ServoKit(channels=8)
while True :
    kit.servo[0].angle = 20
    sleep(.5)
    kit.servo[0].angle = 0
    sleep(.5)
    kit.servo[1].angle = 20
    sleep(.5)
    kit.servo[1].angle = 0
    sleep(.5)
