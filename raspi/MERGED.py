#!/usr/bin/python
import time
import RPi.GPIO as GPIO
from time import sleep

#GPIO BASED
infra_red = 5
led = 6
push_button = 15
i2c = 2 , 3 
servo_check = 18

# Pakai numbering GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(infra_red, GPIO.IN)
GPIO.setup(led, GPIO.OUT)
GPIO.setup(push_button, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(servo_check, GPIO.OUT)

def setAngle(angle):
    duty = angle / 18 + 2
    GPIO.output(12, True)
    pwm.ChangeDutyCycle(duty)
    sleep(1)
    GPIO.output(12, False)
    pwm.ChangeDutyCycle(duty)

def nyala()
   GPIO.output(led, GPIO.HIGH)

def mati()
   GPIO.output(led, GPIO.LOW)

def button_callback(channel): 
   print("Button pressed!")
   setAngle(90)



def kepencet()
def ReadDistance(pin):
   GPIO.setup(pin, GPIO.OUT)
   GPIO.output(pin, 0)

   time.sleep(0.000002)


   

GPIO.add_event_detect(button_pin, GPIO.FALLING, callback=button_callback, bouncetime=300)      
   
try: 
   while True:

      if (GPIO.input(5)==False): #object is far away
         nyala()
      elif (GPIO,input(5)==True): #object is close
         mati()
       
   

except KeyboardInterrupt:
    print("\nExiting program")
    GPIO.cleanup()
    pi.stop()
   
   
