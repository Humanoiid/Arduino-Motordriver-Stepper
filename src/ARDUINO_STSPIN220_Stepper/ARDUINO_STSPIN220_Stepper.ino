
#include <Stepper.h>

int mode_1 = 5; // CN9 pin6 /Arduino 5
int mode_2 = 10; // CN5 pin3 /Arduino 10
int mode_3 = 3; // CN9 pin4 /Arduino 3 (= STCK)
int mode_4 = 7; // CN9 pin8 /Arduino 7

int STEP = 3; // DB STCK PWM1 (DIO 3) (CN9 pin4)
int DIR = 7; // DB DIR1 (CN9 pin8)

const int STBY=8;     // STBY = Standby\reset input. When forced low the device enters in low consumption mode: all motor-coils are non-current 

// motor paramater
int rot_step = 16;
int ustepping = 256; // 1, 2, 4, 8, 16, 32, 64, 128, 256
double rot_step_ustep = rot_step * ustepping;

double rotation_freq = 3; //motor rotation speed [Hz]
double rotation_period = 1000000/rotation_freq; //[us]
float rotation_period_ms = float(rotation_period)/1000; // [ms]

double step1_period = rotation_period/rot_step_ustep; //[us]
double SteppingPRP = 4 * step1_period;  //[us]
double SteppingPRF = 1000000/SteppingPRP; //[Hz] 


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
  Serial.print("rotation_period[ms]: ");
  Serial.println(rotation_period_ms);
  Serial.print("stepping_period[us]: ");
  Serial.println(step1_period);
  Serial.print("HALF stepping_period[us]: ");
  Serial.println(step1_period/2);
  Serial.print("INT HALFstepping_period[us]: ");
  Serial.println(int(step1_period/2));
  Serial.print("number of steps in 1 rotation: ");
  Serial.println(rot_step_ustep);


//  
//  
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(DIR,HIGH); // + Direction
  for(int x = 0; x < 10*rot_step_ustep; x++) {  // 200 pulses one revolution
   digitalWrite(STEP,HIGH); 
   delayMicroseconds(int(step1_period/2)); 
   digitalWrite(STEP,LOW); 
   delayMicroseconds(int(step1_period/2)); 
  }
  delay(1000); 
  
//  digitalWrite(DIR,LOW); // - Direction
//  for(int x = 0; x < 1*rot_step_ustep; x++) {  
//   digitalWrite(STEP,HIGH); 
//   delayMicroseconds(int(step1_period)); 
//   digitalWrite(STEP,LOW); 
//   delayMicroseconds(int(step1_period)); 
//  }
//  delay(1000);
}

void setup_StepperMTDR(int STBY, int mode_1,int mode_2, int mode_3, int mode_4){
  // method of setups shows in Documnets AN4923 (Application note: STSPIN220)
  digitalWrite(STBY,LOW); 
  delayMicroseconds(2); // at least 1us
  
  // check link for ustepping mode
  // https://www.pololu.com/product/2876
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

  
