/* references
 *  
 *  1. ustepping mode: https://www.pololu.com/product/2876
 *  2. audio freq noise & acc/decelation: https://blog.naver.com/motor_bank/221591338372
 *  3. time control: http://rokitarduino.org/2014/05/23/input%EA%B3%BC-input_pullup%EC%9D%98-%EC%B0%A8%EC%9D%B4/
 *  4. register GPIO: http://blog.naver.com/PostView.nhn?blogId=twophase&logNo=221004945877
 *  5. register, gpio, pin informations: https://opentutorials.org/module/1837/10642
 *  
 *  
 *  * micro() function takes around 4 microseconds. 
 *  as I need to control GPIO control around 4 microseconds, use GPIO register & minimum micro() to control time
 *  
 *  * Consider 
 *  
 */
#include <Stepper.h>
#include <TimerOne.h> // timer to reduce audio frequency band noise
#include <MsTimer2.h> // timer for Acceleration/deceleration control


int mode_1 = 5; // CN9 pin6 /Arduino 5 (PD5)
int mode_2 = 10; // CN5 pin3 /Arduino 10 (PB2)
int mode_3 = 3; // CN9 pin4 /Arduino 3 (= STCK)
int mode_4 = 7; // CN9 pin8 /Arduino 7

int STEP = 3; // DB STCK PWM1 (DIO 3) (CN9 pin4) (PD3)
int DIR = 7; // DB DIR1 (CN9 pin8) (PD7)

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
  pinMode(mode_3,OUTPUT); // == pinMode(STEP,OUTPUT); (
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
unsigned long time_3;
void loop() {
  // put your main code here, to run repeatedly:

  // number of rotation
  for(int i = 0; i<10;i++)
  {
    ///// 1 rotation loop
    PORTD = B10000000; // digital pin 7 HIGH
    for(int x = 0; x < rot_step_ustep; x++) {  // 3 rotation
      time_1 = micros(); // t1 = when it start rotation    
      PORTD = B10001000; // digital pin 3 HIGH
      while(time_2 - time_1 < control_time){
        time_2 = micros();   
      }
      PORTD = B10000000; // digital pin 3 LOW
      while(time_3 - time_2 < control_time){
        time_3 = micros();   
      }
    }
    
  }
  // rest
  PORTD = B00000000; // digital pin 7 LOW 
  delay(2000); 
  

}

void setup_StepperMTDR(int STBY, int mode_1,int mode_2, int mode_3, int mode_4){
  // method of setups shows in Documnets AN4923 (Application note: STSPIN220)
  digitalWrite(STBY,LOW); 
  delayMicroseconds(2); // at least 1us
  

  digitalWrite(mode_1,LOW); //
  digitalWrite(mode_2,HIGH); //
  digitalWrite(mode_3,HIGH); //
  digitalWrite(mode_4,LOW); //

  digitalWrite(STBY,HIGH);
  delayMicroseconds(120); // at least 100us

////  digitalWrite(mode_1,HIGH); //
////  digitalWrite(mode_2,HIGH); //
//  digitalWrite(mode_3,HIGH); //  
//  digitalWrite(mode_4,LOW); //
}

  
