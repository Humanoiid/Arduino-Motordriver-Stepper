/* references
 *  
 *  1. ustepping mode: https://www.pololu.com/product/2876
 *  2. audio freq noise & acc/decelation: https://blog.naver.com/motor_bank/221591338372
 *  
 */
#include <Stepper.h>
#include <TimerOne.h> // timer to reduce audio frequency band noise
#include <MsTimer2.h> // timer for Acceleration/deceleration control


int mode_1 = 5; // CN9 pin6 /Arduino 5
int mode_2 = 10; // CN5 pin3 /Arduino 10
int mode_3 = 3; // CN9 pin4 /Arduino 3 (= STCK)
int mode_4 = 7; // CN9 pin8 /Arduino 7

int STEP = 3; // DB STCK PWM1 (DIO 3) (CN9 pin4)
int DIR = 7; // DB DIR1 (CN9 pin8)

const int STBY=8;     // STBY = Standby\reset input. When forced low the device enters in low consumption mode: all motor-coils are non-current 

// motor paramater ====================================================
int rot_step = 16;
int ustepping = 256; // 1, 2, 4, 8, 16, 32, 64, 128, 256
double rot_step_ustep = rot_step * ustepping;

double rotation_freq = 30; //motor rotation speed [Hz]
double rotation_period = 1000000/rotation_freq; //[us]
float rotation_period_ms = float(rotation_period)/1000; // [ms]

double step1_period = rotation_period/rot_step_ustep; //[us]
double SteppingPRP = 4 * step1_period;  //[us]
double SteppingPRF = 1000000/SteppingPRP; //[Hz] 

int control_time = int(step1_period/2); // [us]

void setup() {
  // put your setup code here, to run once:
  pinMode(mode_1,OUTPUT); 
  pinMode(mode_2,OUTPUT); 
  pinMode(mode_3,OUTPUT); // == pinMode(STEP,OUTPUT); 
  pinMode(mode_4,OUTPUT); // == pinMode(DIR,OUTPUT);
  pinMode(STBY, OUTPUT);  

  //// STSPIN220 Motor Driver Setup
  setup_StepperMTDR(STBY, mode_1,mode_2,mode_3,mode_4);

  Serial.begin(9600);
  Serial.println("--- Serial Begin ---");
  Serial.println("==== Given speed ====");
  Serial.print("* Rotation_period[ms]: ");
  Serial.println(rotation_period_ms);
  
  Serial.println("==== Stepper motor Environment ====");
  Serial.print("* # steps: ");
  Serial.println(rot_step);
  Serial.print("* stepping_period[us]: ");
  Serial.println(step1_period);

  Serial.println("==== Micro stepping Environment ====");
  Serial.print("* microstepping(ustep) number: ");
  Serial.println(ustepping);
  Serial.print("* # of total ustep: ");
  Serial.println(rot_step_ustep);
  




  Serial.println("==== Final Stepping condition ====");  
  Serial.print("HALF stepping_period - minimum stepping pulse interval [us]: ");
  Serial.println(step1_period/2);
  Serial.print("INT HALFstepping_period(control time) [us]: ");
  Serial.println(control_time);


//  
//  
}
unsigned long time_res;
unsigned long time_1;
unsigned long time_2;
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(DIR,HIGH); // + Direction
  
  for(int x = 0; x < 1*rot_step_ustep; x++) {  // 200 pulses one revolution
//   time_1 = micros();   
   digitalWrite(STEP,HIGH); 
//   delayMicroseconds(int(step1_period/2)); 
   delayMicroseconds(control_time); 
   time_1 = micros();   
   time_2 = micros();
   
   digitalWrite(STEP,LOW);   
   delayMicroseconds(control_time); 
  }
  time_res = time_2 - time_1;
  Serial.println(time_res);
  delay(1000); 
  

}

void setup_StepperMTDR(int STBY, int mode_1,int mode_2, int mode_3, int mode_4){
  // method of setups shows in Documnets AN4923 (Application note: STSPIN220)
  digitalWrite(STBY,LOW); 
  delayMicroseconds(2); // at least 1us
  

  digitalWrite(mode_1,HIGH); //
  digitalWrite(mode_2,HIGH); //
  digitalWrite(mode_3,LOW); //
  digitalWrite(mode_4,LOW); //

  digitalWrite(STBY,HIGH);
  delayMicroseconds(120); // at least 100us

//  digitalWrite(mode_1,HIGH); //
//  digitalWrite(mode_2,HIGH); //
  digitalWrite(mode_3,HIGH); //  
  digitalWrite(mode_4,LOW); //
}

  
