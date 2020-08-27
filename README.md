# Arduino-Motordriver-Stepper


Arduino + Stepper motor to control small Stepper motor with high angular resolution/ low speed

### Caution
* Some of codes comments are worng, as they are remnant of the things that were being modified.
* Sources list order:  create time 


## Source list 
(different folder by motor)

1. ARDUINO_STEPPER_Direct.ino
 * Directly control Stepper motor by Arduino
 * Show the vibration and some unstable rotation  
2. ARDUINO_DRV8833_Stepper.ino
 * Using Motor driver(DRV8833 for 2 DC motor or Stepper)
 * Can control Stepper with Full step
3. ARDUINO_STSPIN220_Stepper.ino
 * Using Motor driver(STSPIN220 for microstepping with low voltage)
 * can control motor with smooth rotation
 * motor shows strong heat - it seems need to check current or power
4. ARDUINO_STSPIN220_Stepper_manual.ino
 * manually control the time for rotation speed control


## Hardware:

1. Stepper Motor: 
 * model name(?): 18118-P06 cheap stepper motor
 * Link: https://ko.aliexpress.com/item/4000121927885.html?spm=a2g0s.9042311.0.0.4b014c4d8kqu3P
 * Correct pin order by test: A+ A- B+ B-
 * steps of motor by experience: 16 steps 
 
