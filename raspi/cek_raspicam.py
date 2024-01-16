# import libraries
import RPi.GPIO as GPIO
import time
 
# GPIO Modus (BOARD / BCM)
GPIO.setmode(GPIO.BCM)
 
# assign GPIO Pins
GPIO_SIG = 17
 
def distance():
    GPIO.setup(GPIO_SIG, GPIO_OUT)
    GPIO.output(GPIO_SIG, 0)

    time.sleep(0.000002)

    #send trigger signal
    GPIO.output(GPIO_SIG, 1)

    time.sleep(0.000005)

    GPIO.output(GPIO_SIG, 0)

    GPIO.setup(GPIO_SIG, GPIO.IN)

    while GPIO.input(GPIO_SIG) == 0:
        starttime = time.time()

    while GPIO.input(GPIO_SIG) == 1:
        endtime = time.time()
      
    duration = endtime - starttime
    # Distance is defined as time/2 (there and back) * speed of sound 34000 cm/s 
    distance = (duration*34000)/2
 
    return distance
 
if __name__ == '__main__':
    try:
        while True:
            distance = distance()
            print ("Measured distance = %.1f cm" % distance)
            time.sleep(1)
 
        # When canceling with CTRL+C, resetting
    except KeyboardInterrupt:
        print("Measurement stopped by user")
        GPIO.cleanup()