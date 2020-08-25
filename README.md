# Arduino-Motordriver-Stepper
Arduino-STSPIN220-Stepper


## Source list

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

## Hardware:

1 Stepper Motor: 
 * model name(?): 18118-P06 cheap stepper motor
 * Link: https://ko.aliexpress.com/item/4000121927885.html?spm=a2g0s.9042311.0.0.4b014c4d8kqu3P
 * Correct pin order by test: A+ A- B+ B-
 * steps of motor by experience: 16 steps 
 
