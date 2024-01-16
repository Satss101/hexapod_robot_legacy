import math
import time
import varspeed

BASE_HGT = 25
HUMERUS = 103
ULNA = 93
GRIPPER = 140

def ftl(x):
    return int(x + 0.5) if x >= 0 else int(x - 0.5)

BAS_SERVO = 7
SHL_SERVO = 6
ELB_SERVO = 5
WRI_SERVO = 4
WRO_SERVO = 3
GRI_SERVO = 2

hum_sq = HUMERUS * HUMERUS
uln_sq = ULNA * ULNA
servoSpeed = 10

loopCounter = 0
pulseWidth = 6.6
microsecondsToDegrees = 0

def setup():
    global servo1, servo2, servo3, servo4, servo5, servo6
    servo1 = Vspeed()
    servo2 = Vspeed()
    servo3 = Vspeed()
    servo4 = Vspeed()
    servo5 = Vspeed()
    servo6 = Vspeed()
    
    servo1.attach(BAS_SERVO, 544, 2400)
    servo2.attach(SHL_SERVO, 544, 2400)
    servo3.attach(ELB_SERVO, 544, 2400)
    servo4.attach(WRI_SERVO, 544, 2400)
    servo5.attach(WRO_SERVO, 544, 2400)
    servo6.attach(GRI_SERVO, 544, 2400)
    
    servo_park()
    time.sleep(5)
    print("Start")

def loop():
    line()

def set_arm(x, y, z, grip_angle_d, servoSpeed):
    grip_angle_r = math.radians(grip_angle_d)
    
    bas_angle_r = math.atan2(x, y)
    rdist = math.sqrt((x * x) + (y * y))
    
    y = rdist
    
    grip_off_z = (math.sin(grip_angle_r)) * GRIPPER
    grip_off_y = (math.cos(grip_angle_r)) * GRIPPER
    
    wrist_z = (z - grip_off_z) - BASE_HGT
    wrist_y = y - grip_off_y
    
    s_w = (wrist_z * wrist_z) + (wrist_y * wrist_y)
    s_w_sqrt = math.sqrt(s_w)
    
    a1 = math.atan2(wrist_z, wrist_y)
    
    a2 = math.acos(((hum_sq - uln_sq) + s_w) / (2 * HUMERUS * s_w_sqrt))
    
    shl_angle_r = a1 + a2
    shl_angle_d = math.degrees(shl_angle_r)
    
    elb_angle_r = math.acos((hum_sq + uln_sq - s_w) / (2 * HUMERUS * ULNA))
    elb_angle_d = math.degrees(elb_angle_r)
    elb_angle_dn = -(180.0 - elb_angle_d)
    
    wri_angle_d = (grip_angle_d - elb_angle_dn) - shl_angle_d
    
    bas_servopulse = 1500.0 - ((math.degrees(bas_angle_r)) * pulseWidth)
    shl_servopulse = 1500.0 + ((shl_angle_d - 90.0) * pulseWidth)
    elb_servopulse = 1500.0 - ((elb_angle_d - 90.0) * pulseWidth)
    
    wri_servopulse = 1500 - (wri_angle_d * pulseWidth)
    
    microsecondsToDegrees = map(ftl(bas_servopulse), 544, 2400, 0, 180)
    servo1.write(microsecondsToDegrees, servoSpeed)
    
    microsecondsToDegrees = map(ftl(shl_servopulse), 544, 2400, 0, 180)
    servo2.write(microsecondsToDegrees, servoSpeed)
    
    microsecondsToDegrees = map(ftl(elb_servopulse), 544, 2400, 0, 180)
    servo3.write(microsecondsToDegrees, servoSpeed)
    
    microsecondsToDegrees = map(ftl(wri_servopulse), 544, 2400, 0, 180)
    servo4.write(microsecondsToDegrees, servoSpeed)

def servo_park():
    servo1.write(90, 10)
    servo2.write(90, 10)
    servo3.write(90, 10)
    servo4.write(90, 10)
    servo5.write(90, 10)
    servo6.write(80, 10)
    return

def zero_x():
    for yaxis in range(250, 401):
        print(" yaxis= : ", yaxis)
        set_arm(0, yaxis, 200.0, 0, 10)
        time.sleep(0.01)
    for yaxis in range(400, 249, -1):
        set_arm(0, yaxis, 200.0, 0, 10)
        time.sleep(0.01)

def line():
    for xaxis in range(-100, 101, 2):
        set_arm(xaxis, 100, 120, 0, 150)
        time.sleep(0.01)
    for xaxis in range(100, -101, -2):
        set_arm(xaxis, 100, 120, 0, 150)
        time.sleep(0.01)

def circle():
    RADIUS = 50.0
    
    for angle in range(0, 360):
        yaxis = RADIUS * math.sin(math.radians(angle)) + 300
        zaxis = RADIUS * math.cos(math.radians(angle)) + 200
        set_arm(0, yaxis, zaxis, 0, 50)
        time.sleep(0.01)


