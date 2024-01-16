#Example Servo Code
#Control the angle of a 
#Servo Motor with Raspberry Pi

# free for use without warranty
# www.learnrobotics.org

import RPi.GPIO as GPIO
from time import sleep
servo = [12,3]
srf = [17,27,22,10,9,11]
i = 1
GPIO.setmode(GPIO.BOARD)
GPIO.setup(servo[0], GPIO.OUT)
pwm=GPIO.PWM(servo[0], 60)
pwm.start(0)

for i in range(6):
   GPIO.setup(srf[i], GPIO.OUT)

def setAngle(id, angle):
    duty = angle / 18 + 2
    GPIO.output(servo[id], True)
    pwm.ChangeDutyCycle(duty)
    sleep(1)
    GPIO.output(servo[id], False)
    pwm.ChangeDutyCycle(duty)


def ReadDistance(pin):
   GPIO.setup(srf[pin], GPIO.OUT)
   GPIO.output(srf[pin], 0) 

   time.sleep(0.000002)
   #send trigger signal
   GPIO.output(srf[pin], 1)
   time.sleep(0.000005)
   GPIO.output(srf[pin], 0)

   GPIO.setup(srf[pin], GPIO.IN)
   while GPIO.input(srf[pin])==0:
      starttime=time.time()
   while GPIO.input(srf[pin])==1:
      endtime=time.time()
      
   duration=endtime-starttime
   # Distance is defined as time/2 (there and back) * speed of sound 34000 cm/s 
   distance=duration*34000/2
   return distance
    
    

count = 0
numLoops = 2

while count < numLoops:
    print("set to 0-deg")
    setAngle(0, 0)
    sleep(1)

        
    print("set to 90-deg")
    setAngle(0, 90)
    sleep(1)

    print("set to 135-deg")
    setAngle(0, 135)
    sleep(1)
    
    count=count+1

pwm.stop()
GPIO.cleanup()

