# Libraries 
from picobricks import WS2812, DHT11, NEC_16
from machine import Pin, PWM, ADC, UART
import time
import utime

# Frequencies and duration for melody
melody = [262, 330, 392, 523] # Notes (Hz)
duration = 0.4 # Note duration in seconds

windowOpen = 2000
windowClose = 4000
doorOpen = 2000
doorClose = 4500

tempThreshold = 25
ldrThreshold = 8000

# Pin configurations
ws2812 = WS2812(1,brightness=1)  # RGB LED strip
ldr = ADC(Pin(27))  # Light sensor (LDR)
dht_sensor = DHT11(Pin(11))  # Temperature & humidity sensor
buzzer = Pin(20, Pin.OUT)  # Buzzer
button = Pin(10, Pin.IN)  # Button input
led = Pin(7, Pin.OUT)  # LED output

# Servo motor for the door
servo1 =PWM(Pin(21,Pin.OUT))
servo1.freq(50)
servo1.duty_u16(doorClose) # Initial position (closed)

# Servo motor for the window
servo2 =PWM(Pin(22,Pin.OUT))
servo2.freq(50)
servo2.duty_u16(windowClose) # Initial position (closed)

# Variable to store the last time DHT11 temperature and humidity were measured
dht_read_time = time.time()

def buzzer_tone(frequency, duration):
    period = 1.0 / frequency  # Wave period (seconds)
    cycles = int(duration * frequency)  # Number of cycles

    for _ in range(cycles):
        buzzer.value(1)  # HIGH (sound ON)
        time.sleep(period / 2)
        buzzer.value(0)  # LOW (sound OFF)
        time.sleep(period / 2)

def play_melody():
    for note in melody:
        buzzer_tone(note, duration)
        time.sleep(0.1)  # Short pause between notes

# Interrupt function for button press
def buttonInterruptHandler(event):    
    if button.value() == 1:
        led.high()  # Turn on LED
        time.sleep(1)
        play_melody()  # Play doorbell melody
        led.low()  # Turn off LED
        servo1.duty_u16(doorOpen)  # Open door
        time.sleep(2)
        servo1.duty_u16(doorClose)  # Close door
        
# Configure button interrupt
button.irq(trigger=Pin.IRQ_RISING, handler=buttonInterruptHandler)

while True:
    # Automatic lighting with LDR sensor
    ldr_value = ldr.read_u16()
    if ldr_value > ldrThreshold:
        ws2812.pixels_fill((255, 255, 255))  # Turn on lights
        ws2812.pixels_show()
    elif ldr_value <= ldrThreshold:
        ws2812.pixels_fill((0, 0, 0))  # Turn off lights
        ws2812.pixels_show()
        
    # Read temperature from DHT11 sensor every 3 seconds
    if time.time() - dht_read_time >= 3:
        dht_read_time = time.time()
    try:
        dht_sensor.measure()
    except Exception as e:
        pass  # Ignore errors if sensor fails
        
    temperature = dht_sensor.temperature
    
    #print("TEMP:     {0:.2f}C".format(temperature))
    
    # Automatically open/close the window based on temperature
    if temperature >= tempThreshold:
        servo2.duty_u16(windowOpen)  # Open window
    elif temperature < tempThreshold:
        servo2.duty_u16(windowClose)  # Close window

