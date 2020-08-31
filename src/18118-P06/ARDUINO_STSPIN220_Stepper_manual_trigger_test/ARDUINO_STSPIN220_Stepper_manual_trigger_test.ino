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
//#include <AccelStepper.h> // accelation control

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
int ustepping = 4; // 1, 2, 4, 8, 16, 32, 64, 128, 256
double rot_step_ustep = rot_step * ustepping;

double rotation_freq = 1; //motor rotation speed [Hz]
double rotation_period = 1000000/rotation_freq; //[us]
double rotation_period_ms = double(rotation_period)/1000; // [ms]

double step1_period = rotation_period/rot_step_ustep; //[us]
//double SteppingPRP = 4 * step1_period;  //[us]
//double SteppingPRF = 1000000/SteppingPRP; //[Hz] 

long control_time = long(step1_period/2); // [us]

// parameter for variable control
unsigned long time_1; // start pulse
unsigned long time_2; // first pulse
unsigned long time_3; // second pulse
unsigned long time_temp; // second pulse

unsigned long timel_1; // loop1
unsigned long timel_2; // loop2
int val;
int stat_stanby=0;
int steps;
// function
void setup_StepperMTDR(int STBY, int mode_1,int mode_2, int mode_3, int mode_4);

void setup() {
  // put your setup code here, to run once:
  pinMode(mode_1,OUTPUT); 
  pinMode(mode_2,OUTPUT); 
  pinMode(mode_3,OUTPUT); // == pinMode(STEP,OUTPUT); (
  pinMode(mode_4,OUTPUT); // == pinMode(DIR,OUTPUT);
  pinMode(STBY, OUTPUT);  
  pinMode(inPin, INPUT_PULLUP); // pullup to set default HIGH (if connect with GND, LOW)

  //// STSPIN220 Motor Driver Setup
  setup_StepperMTDR(STBY, mode_1,mode_2,mode_3,mode_4);
  digitalWrite(STBY,LOW);
  stat_stanby = 0;

  Serial.begin(9600);
  Serial.println("------------------ Serial Begin ------------------");
  Serial.println("==== Given speed ====");
  Serial.print("* Rotation_period[ms]: ");
  Serial.print(rotation_period_ms);
  Serial.print("/ Rotation_speed[Hz]: ");
  Serial.println(rotation_freq);
  
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
}

void loop() {
  // if there are input, rotate
  timel_1 = micros();
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
    for(int i = 0; i<1;i++)
    {
      timel_1 = micros();
      ///// 1 rotation loop
      PORTD = B10000000; // digital pin 7 HIGH
//      time_temp = micros();
      for(int x = 0; x < rot_step_ustep; x++) {  // 3 rotation
        time_1 = micros(); // t1 = when it start rotation 
        time_2 = time_1;
        PORTD = B10001000; // digital pin 3 HIGH
        while(time_2 - time_1 < control_time){
          time_2 = micros(); 
        } 
        time_3 = time_2;
        PORTD = B10000000; // digital pin 3 LOW
        while(time_3 - time_2 < control_time){
          time_3 = micros();   
        } 
//        analogWrite(STEP,255);
//        analogWrite(STEP,0);
//        digitalWrite(STEP,HIGH);
//        digitalWrite(STEP,LOW);
//          PORTD = B10001000; // digital pin 3 HIGH
//          PORTD = B10000000; // digital pin 3 LOW
//          PORTD = B10001000; // digital pin 3 HIGH
//          PORTD = B10000000; // digital pin 3 LOW
        delay(10000/rot_step_ustep);
        Serial.print("steps: ");
        Serial.println(x);
      }
      timel_2 = micros();
      Serial.print("1 rotate [us]: ");
      Serial.print(timel_2 - timel_1); 
      Serial.print("/ Rotation speed [Hz]: ");
      Serial.println(1000000/double(timel_2 - timel_1)); 
 
//      Serial.print("1 insert [us]: ");
//      Serial.println(timel_1 - time_temp); 
//      steps = 0;
//      while(steps++ != rot_step_ustep)
//      {
//        PORTD = B10001000; // digital pin 3 HIGH
//        PORTD = B10000000; // digital pin 3 LOW
//        fast_toggle();
//      }
    }

    
    // rest
    PORTD = B00000000; // digital pin 7 LOW 
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

//  Serial.print("times [us]: ");
//  Serial.println(timel_2 - timel_1);
}

void setup_StepperMTDR(int STBY, int mode_1,int mode_2, int mode_3, int mode_4){
  // method of setups shows in Documnets AN4923 (Application note: STSPIN220)
  digitalWrite(STBY,LOW); 
  delayMicroseconds(2); // at least 1us
  

//  digitalWrite(mode_1,LOW); //
//  digitalWrite(mode_2,HIGH); //
//  digitalWrite(mode_3,HIGH); //
//  digitalWrite(mode_4,LOW); //
  
  digitalWrite(mode_1,LOW); //
  digitalWrite(mode_2,HIGH); //
  digitalWrite(mode_3,LOW); //
  digitalWrite(mode_4,HIGH); //

  digitalWrite(STBY,HIGH);
  delayMicroseconds(120); // at least 100us

////  digitalWrite(mode_1,HIGH); //
////  digitalWrite(mode_2,HIGH); //
//  digitalWrite(mode_3,HIGH); //  
//  digitalWrite(mode_4,LOW); //
}

void fast_toggle(){
  PORTD = B10001000; // digital pin 3 HIGH
  PORTD = B10000000; // digital pin 3 LOW
}
  
