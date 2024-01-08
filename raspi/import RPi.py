import RPi.GPIO as GPIO
import time

# Set the GPIO mode
GPIO.setmode(GPIO.BCM)

# Define GPIO pins for the LED and button
led_pin = 18
button_pin = 17

# Set up GPIO for the LED
GPIO.setup(led_pin, GPIO.OUT)

# Set up GPIO for the button with pull-down resistor
GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

# Function to turn on the LED
def turn_on_led():
    GPIO.output(led_pin, GPIO.HIGH)

# Callback function for button press
def button_callback(channel):
    print("Button pressed!")
    turn_on_led()

# Add event listener for button press
GPIO.add_event_detect(button_pin, GPIO.FALLING, callback=button_callback, bouncetime=300)

try:
    print("Press Ctrl+C to exit")
    while True:
        time.sleep(1)

except KeyboardInterrupt:
    print("\nExiting program")
    GPIO.cleanup()
