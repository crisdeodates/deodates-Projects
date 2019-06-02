!/usr/bin/python
#----------------------------------------------------------------------------#
#-- PROJECT Autonomous Fire Response & Monitoring Unit             
#
#
#-- PROGRAM GOALS                                                               #
#-- 1. Control a ground vehicle autonomously or manually using mobile app       #
#-- 2. Detect obstacles and provide warnings                                    #
#                                                                               #
#-- Programmers: Cris Thomas, Jiss Joseph Thomas                                #
#-- References: Donkey Car                                                      #
#----------------------------------------------------------------------------#
import socket
import sys
import atexit
import RPi.GPIO as GPIO
import time
import threading
from Raspi_MotorHAT import Raspi_MotorHAT, Raspi_DCMotor
from Raspi_PWM_Servo_Driver import PWM
from control import *
from IR import *
def get_rx_message():
   global data_rx_temp
   data_rx_temp_lock = threading.Lock()
   global UDP_IP, UDP_PORT
   # Create a UDP socket
   try:
       sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
       print('Socket created')
   except socket.error as msg:
       print('Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
       sys.exit()
   # Bind the socket to the port
   server_address = (UDP_IP, UDP_PORT)
   print >> sys.stderr, 'starting up on %s port %s' % server_address
   try:
       sock.bind(server_address)
   except socket.error as msg:
       print('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
       sys.exit()
   print('Socket bind complete')
   atexit.register(sock.close)
   while True:
       print >> sys.stderr, '\nwaiting to receive message'
       with data_rx_temp_lock:
           data_rx_temp, address = sock.recvfrom(4096)
def do_control():
   global car_fwd_back, car_left_right, set_speed
   atexit.register(turnOffMotors)
   atexit.register(reset_all_servos)
   while True:
       global cmd_mode, control_mode
       if cmd_mode == 0 and control_mode == 0:  # Manual Cam
           do_cam_control()
       elif cmd_mode == 0 and control_mode == 1:  # Manual Car
           do_car_control(car_fwd_back, car_left_right, set_speed)
       elif cmd_mode == 1 and control_mode == 0:  # Manual Cam + Auto OFF
           do_cam_control()
       elif cmd_mode == 1 and control_mode == 1:  # Manual Cam + Auto ON
           do_autonomous()
           do_cam_control()
def do_autonomous():
   global object_left, object_front, object_right
   get_IR_data()
   control_lock = threading.Lock()
   with control_lock:
       object_left_temp = object_left
       object_front_temp = object_front
       object_right_temp = object_right
   if not object_front_temp:
       motor_fwd_back.run(Raspi_MotorHAT.FORWARD)
       motor_fwd_back_2.run(Raspi_MotorHAT.FORWARD)
       motor_left_right.run(Raspi_MotorHAT.RELEASE)
       print("\nAuto : Moving forward")
   elif not object_right_temp:
       print("\nAuto : Turning right")
       motor_left_right.run(Raspi_MotorHAT.FORWARD)
       motor_fwd_back.run(Raspi_MotorHAT.FORWARD)
       motor_fwd_back_2.run(Raspi_MotorHAT.FORWARD)
   elif not object_left_temp:
       print("\nAuto : Turning left")
       motor_left_right.run(Raspi_MotorHAT.BACKWARD)
       motor_fwd_back.run(Raspi_MotorHAT.FORWARD)
       motor_fwd_back_2.run(Raspi_MotorHAT.FORWARD)
   else:
       motor_fwd_back.run(Raspi_MotorHAT.BACKWARD)
       motor_fwd_back_2.run(Raspi_MotorHAT.BACKWARD)
       time.sleep(1)
       motor_fwd_back.run(Raspi_MotorHAT.RELEASE)
       motor_fwd_back_2.run(Raspi_MotorHAT.RELEASE)
       print("\nAuto : Moving Back for re-route")
       if not object_right_temp:
           motor_left_right.run(Raspi_MotorHAT.FORWARD)
           motor_fwd_back.run(Raspi_MotorHAT.FORWARD)
           motor_fwd_back_2.run(Raspi_MotorHAT.FORWARD)
           time.sleep(1)
           print("\nAuto : Turning right for re-route")
       elif not object_left_temp:
           motor_left_right.run(Raspi_MotorHAT.BACKWARD)
           motor_fwd_back.run(Raspi_MotorHAT.FORWARD)
           motor_fwd_back_2.run(Raspi_MotorHAT.FORWARD)
           time.sleep(1)
           print("\nAuto : Turning left for re-route")

########################## MAIN PROGRAM ##############################

# create a default object, no changes to I2C address or frequency
mh = Raspi_MotorHAT(addr=0x6F)
# Initialise the PWM device using the default address
pwm = PWM(0x6F)
# Front motor
motor_fwd_back = mh.getMotor(1)
# Back motor
motor_fwd_back_2 = mh.getMotor(2)
# Turn motor Front to left right
motor_left_right = mh.getMotor(3)
# Speed in range of 0 - 255
motor_fwd_back.setSpeed(50)
motor_fwd_back_2.setSpeed(50)
motor_left_right.setSpeed(50)
pwm.setPWMFreq(60)  # Set frequency to 60 Hz
reset_all_servos()
turnOffMotors()
# get_rx_message()
thread_rx = threading.Thread(target=get_rx_message)
# do_control
thread_control = threading.Thread(target=do_control)
thread_rx.setDaemon(True)
thread_control.setDaemon(True)
thread_rx.start()
thread_control.start()
thread_rx.join()
thread_control.join()
GPIO.cleanup()
