from machine import Pin, PWM
import time

servo_door = PWM(Pin(21))    # MOTOR_1
servo_window = PWM(Pin(22))  # MOTOR_2

servo_door.freq(50)
servo_window.freq(50)

doorOpen    = 2000
doorClose   = 4500
windowOpen  = 2000
windowClose = 4000

CENTER = 3276 

def servo_write(servo, duty, wait=0.4):
    duty = max(1500, min(8000, int(duty))) 
    servo.duty_u16(duty)
    time.sleep(wait)

servo_write(servo_door, CENTER)
servo_write(servo_window, CENTER)

servo_write(servo_door, doorClose)
servo_write(servo_window, windowClose)

print("PicoBricks Servo Calibration Done")
