#!/usr/bin/python
import time
import RPi.GPIO as GPIO
from time import sleep

#GPIO BASED
infra_red = 5
led = 6
push_button = 15
i2c = [2,3]
srf = [17,27,22,10,9,11]
servo = [12,3]

# Pakai numbering GPIO
GPIO.setmode(GPIO.BOARD)
# GPIO.setmode(GPIO.BCM)
# GPIO.setmode(GPIO.BOARD)   #pilih yang atas apa bawah
GPIO.setup(infra_red, GPIO.IN)
GPIO.setup(led, GPIO.OUT)
GPIO.setup(push_button, GPIO.OUT)
#GPIO.setup(push_button, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
# GPIO.setup(servo_check, GPIO.OUT)

pwm.start(0)
for i in range(6):
   GPIO.setup(srf[i], GPIO.OUT)
   GPIO.setup(servo[i], GPIO.OUT)
   pwm=GPIO.PWM(servo[i], 60)
# ----------------------------------

def setAngle(id, angle):
    duty = angle / 18 + 2
    GPIO.output(servo[id], True)
    pwm.ChangeDutyCycle(duty)
    sleep(1)
    GPIO.output(servo[id], False)
    pwm.ChangeDutyCycle(duty)
    
def bacaIR() :
	return GPIO.input(infra_red)
	    
def bacaPB() :
	return GPIO.input(push_button)
	
def ledNyala() :
   GPIO.output(led, True)

def ledMati() :
   GPIO.output(led, False)

#def button_callback(channel): 
   #print("Button pressed!")
   #setAngle(90)

# def kepencet()

def ReadDistance(pin):
   GPIO.setup(pin, GPIO.OUT)
   GPIO.output(pin, 0) 
   time.sleep(0.000002)
   #send trigger signal
   GPIO.output(pin, 1)
   time.sleep(0.000005)
   GPIO.output(pin, 0)
   GPIO.setup(pin, GPIO.IN)
   while GPIO.input(pin)==0:
      starttime=time.time()
   while GPIO.input(pin)==1:
      endtime=time.time()
   duration=endtime-starttime
   # Distance is defined as time/2 (there and back) * speed of sound 34000 cm/s 
   distance=duration*34000/2
   return distance

#GPIO.add_event_detect(button_pin, GPIO.FALLING, callback=button_callback, bouncetime=300)      
   
#try: 
while True:
    if (bacaIR() == False): #object is far away
        ledNyala()
    elif (GbacaIR() == True): #object is close
        ledMati()
       
   

#except KeyboardInterrupt:
#    print("\nExiting program")
#    GPIO.cleanup()
#    pi.stop()
   
   
