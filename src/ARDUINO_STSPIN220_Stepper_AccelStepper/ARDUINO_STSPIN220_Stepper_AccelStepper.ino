/* references
 *  
 *  1. ustepping mode: https://www.pololu.com/product/2876
 *  2. audio freq noise & acc/decelation: https://blog.naver.com/motor_bank/221591338372
 *  3. time control: http://rokitarduino.org/2014/05/23/input%EA%B3%BC-input_pullup%EC%9D%98-%EC%B0%A8%EC%9D%B4/
 *  4. register GPIO: http://blog.naver.com/PostView.nhn?blogId=twophase&logNo=221004945877
 *  5. register, gpio, pin informations: https://opentutorials.org/module/1837/10642
 *  6. AccelStepper Lib: https://makernambo.com/66
 *  
 *  * micro() function takes around 4 microseconds. 
 *  as I need to control GPIO control around 4 microseconds, use GPIO register & minimum micro() to control time
 *  
 *  * Consider 
 *  
 */
#include <Stepper.h>
//#include <TimerOne.h> // timer to reduce audio frequency band noise
//#include <MsTimer2.h> // timer for Acceleration/deceleration control
#include <AccelStepper.h>

int mode_1 = 5; // CN9 pin6 /Arduino 5 (PD5)
int mode_2 = 10; // CN5 pin3 /Arduino 10 (PB2)
int mode_3 = 3; // CN9 pin4 /Arduino 3 (= STCK)
int mode_4 = 7; // CN9 pin8 /Arduino 7

int STEP = 3; // DB STCK PWM1 (DIO 3) (CN9 pin4) (PD3)
int DIR = 7; // DB DIR1 (CN9 pin8) (PD7)

const int STBY=8;     // STBY = Standby\reset input. When forced low the device enters in low consumption mode: all motor-coils are non-current 

int inPin = 13; // read state to rotate or not

// motor paramater ====================================================
int rot_step = 16;
int ustepping = 256; // 1, 2, 4, 8, 16, 32, 64, 128, 256
double rot_step_ustep = rot_step * ustepping;

double rotation_freq = 1; //motor rotation speed [Hz]
double rotation_period = 1000000/rotation_freq; //[us]
float rotation_period_ms = float(rotation_period)/1000; // [ms]

double step1_period = rotation_period/rot_step_ustep; //[us]

long control_time = long(step1_period/2); // [us]

// parameter for variable control
unsigned long time_1; // start pulse
unsigned long time_2; // first pulse
unsigned long time_3; // second pulse

unsigned long timel_1; // loop1
unsigned long timel_2; // loop2
int val;
int stat_stanby=0;


// AccelStpeer
AccelStepper stepper(AccelStepper::DRIVER, STEP, DIR);

void setup() {
  // put your setup code here, to run once:
  pinMode(mode_1,OUTPUT); 
  pinMode(mode_2,OUTPUT); 
  pinMode(mode_3,OUTPUT); // == pinMode(STEP,OUTPUT); (
  pinMode(mode_4,OUTPUT); // == pinMode(DIR,OUTPUT);
  pinMode(STBY, OUTPUT);  
  pinMode(inPin, INPUT_PULLUP); // pullup to set default HIGH (if connect with GND, LOW)



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

  // AccelStepper init

  stepper_init();    
  stepper.setCurrentPosition(0);  
//  stepper.setSpeed(rotation_freq * rot_step_ustep); // step/seconds..30 Hz -> rotation_freq * rot_step_ustep / 1
  stepper.setSpeed(rotation_freq * rot_step); // step/seconds..30 Hz -> rotation_freq * rot_step_ustep / 1
//  stepper.targetPosition(rot_step_ustep);
}

void loop() {
  // if there are input, rotate
//  timel_1 = micros();
  val = digitalRead(inPin);
//  Serial.println(val);
  
  if(digitalRead(inPin) == LOW){

    //// STSPIN220 Motor Driver Setup
    if(stat_stanby == 0){      
    setup_StepperMTDR(STBY, mode_1,mode_2,mode_3,mode_4);
    stat_stanby = 1;
    Serial.println("On");
    }
    
    // number of rotation
    for(int i = 0; i<2;i++)
    {

      time_1 = micros();
      stepper.moveTo(stepper.currentPosition()+rot_step_ustep);
      while (stepper.currentPosition()  != stepper.targetPosition()){
        stepper.runSpeedToPosition();  
      }
      time_2 = micros();
        Serial.print("times [us]: ");
        Serial.println(time_2 - time_1);

    }
    // rest
//    PORTD = B00000000; // digital pin 7 LOW 
//    delay(2000);
  }
  else{
    if(stat_stanby == 1){
      digitalWrite(STBY,LOW);
      stat_stanby = 0;
      Serial.println("OFF");
    }
  }
//  timel_2 = micros();
//
//  Serial.print("times [us]: ");
//  Serial.println(timel_2 - timel_1);
}

void stepper_init(){   
//  stepper.setMaxSpeed(2000);   
  stepper.setAcceleration(1500);   
}  

void setup_StepperMTDR(int STBY, int mode_1,int mode_2, int mode_3, int mode_4){
  // method of setups shows in Documnets AN4923 (Application note: STSPIN220)
  digitalWrite(STBY,LOW); 
  delayMicroseconds(2); // at least 1us
  

  digitalWrite(mode_1,LOW); //
  digitalWrite(mode_2,HIGH); //
  digitalWrite(mode_3,HIGH); //
  digitalWrite(mode_4,LOW); //
//  digitalWrite(mode_1,LOW); //
//  digitalWrite(mode_2,HIGH); //
//  digitalWrite(mode_3,LOW); //
//  digitalWrite(mode_4,HIGH); //

  digitalWrite(STBY,HIGH);
  delayMicroseconds(120); // at least 100us

////  digitalWrite(mode_1,HIGH); //
////  digitalWrite(mode_2,HIGH); //
//  digitalWrite(mode_3,HIGH); //  
//  digitalWrite(mode_4,LOW); //
}

  
