# -*- coding: utf-8 -*-
"""
Created on Mon Aug 29 19:55:48 2016

@author: PC-BRO

dictionary:
    theta0: 1950 --> 90deg , 21.6666 --> 1deg
    theta1: 1450 --> 90deg , 16.1111 --> 1deg
    theta2: -640 --> 40deg , -16 --> 1deg

"""

import numpy as np
import time
from platform_communication_handler import platformCommunicationHandler as pch

draw_skip = 1
d1 = 0.22
d2 = 0.22
#theta2enc = [21.6666, 16.1111, -16, 0 , 0, 0]
theta2enc = [21.6666, 16.6666, -16.666, 0 , 0, 0]
l_gripper_and_pen = [0.25,0]
l_initY = 0
l_initZ = 0
l_org_to_screen =  0.51
counter_contour = 0
def xyz2angles(x,y,z):
    global d1,d2,theta2enc,l_gripper_and_pen
    theta0 = np.arctan2(z,x)
    theta0_deg = np.rad2deg(theta0)
    x_corr = x/np.cos(theta0)   
    x0 = float(x_corr - l_gripper_and_pen[0])
    y0 = float(y - l_gripper_and_pen[1])
    L = float(np.sqrt(x0*x0 + y0*y0))    
    gamma = np.arccos( -( (L*L - d1*d1 - d2*d2)/(2.0*d1*d2) ) ) 
    alpha = np.arcsin( (d2/L)*np.sin(gamma) )
    theta1 = alpha + np.arctan2(y0,x0)     
    theta1_deg = 90 - np.rad2deg(theta1)
    x1 = d1*np.cos(theta1)
    y1 = d1*np.sin(theta1)
    theta2 = np.arctan2((y0-y1),(x0-x1))
    theta2_deg = -np.rad2deg(theta2)
    #print x,y,z,x_corr,theta0_deg,theta1_deg,theta2_deg
    #print x,y,x0,y0,L,gamma,alpha,theta1,theta1_deg,theta2,theta2_deg,x1,y1
    return [theta0_deg*theta2enc[0],theta1_deg*theta2enc[1],theta2_deg*theta2enc[2],0,0,0]


#def plotLine()

encoder_tmp = [0]*6
encoder_abs_pos_array = [0]*6
encoder_init_positions_array = [0]*6
encoder_poly_array = [0]*37
theta_array = np.zeros([101,6])
colorPitch = [203,380]
colorNow = 0

platform_controller = pch('COM3')
time.sleep(3)
print "Init Done!"
# {false, false, true, false, true, true}

encoder_init_positions_array[0] = 1
encoder_init_positions_array[2] = 1
encoder_init_positions_array[4] = 1
encoder_init_positions_array[5] = 1  

 
platform_controller.give_commands_go_home(encoder_init_positions_array)
platform_controller.read_from_serial_blocking(1024)
time.sleep(8)

# 40  ,  75  ,  0  ,  350  ,  350  ,  0
encoder_abs_pos_array[3] = colorPitch[colorNow]
platform_controller.give_commands(encoder_abs_pos_array) 

platform_controller.read_from_serial_blocking(1024)
time.sleep(5)

"""
flagUp = 0
y = -999
z = -999
lines = open('./files/dino.gcode','r').readlines()
counter = 0


for line in lines:
    if len(line.strip()) > 0:
        z,y = np.array(line.split(',')).astype('float').tolist()
        #print z,y
        if flagUp:
            flagUp = 0
            x = (l_org_to_screen - 0.05)
            encoder_tmp = xyz2angles(x,y,z)
            encoder_abs_pos_array[0:3] = encoder_tmp[0:3]
            platform_controller.give_commands(encoder_abs_pos_array)
            platform_controller.read_from_serial_blocking(1024)
            time.sleep(0.5)
        if np.mod(counter,1) == 0:
            if y < 0:
                addX = y/5
            else:
                addX = 0             
            x = (l_org_to_screen) - addX
            encoder_tmp = xyz2angles(x,y,z)
            encoder_abs_pos_array[0:3] = encoder_tmp[0:3]
            platform_controller.give_commands(encoder_abs_pos_array)
            platform_controller.read_from_serial_blocking(1024)
            time.sleep(0.1)
        counter = counter + 1
    else:
        print 'UP'
        if y == -999:
            flagUp = 1
            continue
        x = (l_org_to_screen - 0.05)
        encoder_tmp = xyz2angles(x,y,z)
        encoder_abs_pos_array[0:3] = encoder_tmp[0:3]
        platform_controller.give_commands(encoder_abs_pos_array)
        platform_controller.read_from_serial_blocking(1024)
        time.sleep(0.1)   
        if counter_contour > 0:
            colorNow = 1 - colorNow
            counter_contour = counter_contour-1
        encoder_abs_pos_array[3] = colorPitch[colorNow]
        platform_controller.give_commands(encoder_abs_pos_array)
        platform_controller.read_from_serial_blocking(1024)
        time.sleep(0.1)   
        flagUp = 1





encoder_init_positions_array[0] = 0
encoder_init_positions_array[2] = 1
encoder_init_positions_array[4] = 1
encoder_init_positions_array[5] = 1  

 
platform_controller.give_commands_go_home(encoder_init_positions_array)
platform_controller.read_from_serial_blocking(1024)
time.sleep(8)

colorNow = 0
encoder_abs_pos_array[3] = colorPitch[colorNow]
platform_controller.give_commands(encoder_abs_pos_array) 
"""

flagUp = 0
y = -999
z = -999
lines = open('./files/pig.gcode','r').readlines()
counter = 0

for line in lines:
    if len(line.strip()) > 0:
        z,y = np.array(line.split(',')).astype('float').tolist()
        #print z,y
        if flagUp:
            flagUp = 0
            x = (l_org_to_screen - 0.05)
            encoder_tmp = xyz2angles(x,y,z)
            encoder_abs_pos_array[0:3] = encoder_tmp[0:3]
            platform_controller.give_commands(encoder_abs_pos_array)
            platform_controller.read_from_serial_blocking(1024)
            time.sleep(0.5)
        if np.mod(counter,draw_skip) == 0:
            if y < 0:
                addX = y/5
            else:
                addX = 0             
            x = (l_org_to_screen) - addX
            encoder_tmp = xyz2angles(x,y,z)
            encoder_abs_pos_array[0:3] = encoder_tmp[0:3]
            platform_controller.give_commands(encoder_abs_pos_array)
            platform_controller.read_from_serial_blocking(1024)
            time.sleep(0.1)
        counter = counter + 1
    else:
        print 'UP'
        if y == -999:
            flagUp = 1
            continue
        x = (l_org_to_screen - 0.05)
        encoder_tmp = xyz2angles(x,y,z)
        encoder_abs_pos_array[0:3] = encoder_tmp[0:3]
        platform_controller.give_commands(encoder_abs_pos_array)
        platform_controller.read_from_serial_blocking(1024)
        time.sleep(0.1)   
        if counter_contour > 0:
            colorNow = 1 - colorNow
            counter_contour = counter_contour-1
        encoder_abs_pos_array[3] = colorPitch[colorNow]
        platform_controller.give_commands(encoder_abs_pos_array)
        platform_controller.read_from_serial_blocking(1024)
        time.sleep(0.1)   
        flagUp = 1
        


"""
y_start = l_initY
z_start = l_initZ
circle_radius = 0.1
delta = 0.002
x = (l_org_to_screen)

timePoly = 5;
counter = 0;
t = np.arange(0,timePoly,5.0/101.0)



for idx in range(-50,51):
    y = y_start - delta*idx
    z = np.sqrt(circle_radius*circle_radius - (y-y_start)*(y-y_start))
    encoder_tmp = xyz2angles(x,y,z)
    theta_array[counter,:] = np.array(encoder_tmp)
  #  print(encoder_tmp)
    counter = counter + 1
#    encoder_abs_pos_array[0:3] = encoder_tmp[0:3]
#    platform_controller.give_commands(encoder_abs_pos_array) 
#    platform_controller.read_from_serial_blocking(1024)
#    time.sleep(0.1)    
#    if idx == 0:
#        print "half!!"

polyfitDeg = 5

for thetaIdx in range(0,3):
    encoder_poly_array[(thetaIdx*6+6-polyfitDeg-1):(thetaIdx*6+6)] = np.polyfit(t,theta_array[:,thetaIdx],polyfitDeg)
    
encoder_poly_array = (np.round(np.multiply(encoder_poly_array,10)))
encoder_poly_array[36] = timePoly
platform_controller.give_commands_poly(encoder_poly_array) 
platform_controller.read_from_serial_blocking(1024)
time.sleep(10)

counter = 0;
for idx in range(50,-51,-1):
    y = y_start - delta*idx
    z = -1*np.sqrt(circle_radius*circle_radius - (y-y_start)*(y-y_start))
    encoder_tmp = xyz2angles(x,y,z)
    theta_array[counter,:] = np.array(encoder_tmp)
  #  print(encoder_tmp)
    counter = counter + 1
#    encoder_abs_pos_array[0:3] = encoder_tmp[0:3]
#    platform_controller.give_commands(encoder_abs_pos_array) 
#    platform_controller.read_from_serial_blocking(1024)
#    time.sleep(0.1)   
#    if idx == 0:
#        print "half!!"


polyfitDeg = 5

for thetaIdx in range(0,3):
    encoder_poly_array[(thetaIdx*6+6-polyfitDeg-1):(thetaIdx*6+6)] = np.polyfit(t,theta_array[:,thetaIdx],polyfitDeg)
    
encoder_poly_array = (np.round(np.multiply(encoder_poly_array,10)))
encoder_poly_array[36] = timePoly
platform_controller.give_commands_poly(encoder_poly_array) 
platform_controller.read_from_serial_blocking(1024)
time.sleep(10)
"""