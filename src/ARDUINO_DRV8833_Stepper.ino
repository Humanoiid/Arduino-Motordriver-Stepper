/*

   [running mode]: Full step drive
   reference link: https://en.wikipedia.org/wiki/Stepper_motor

   speed
   https://arsviator.blogspot.com/2016/01/arduino-uno-digitalwrite.html
   https://blog.naver.com/PostView.nhn?blogId=twophase&logNo=221004945877

   need to contol register directly (digital write is too slow... around 6.5 us... can not give pulse)
*/


#include <Stepper.h>

int ain = 8;
int amin = 9;
int bin = 10;
int bmin = 11;

int num[4] = {ain, bin, amin, bmin};

// fixed parameters
int rot_step = 16; // motor number of steps/rotation
double PRF_drv8833 = 50000; // PRF from DRV8833(mt driver) for stepper motor (by experience) = 50000 Hz (20us)

// parameter we control
int sped_rot = 30; // motor rotation speed = 30Hz

// calculated parameters
int pulses = int(floor(PRF_drv8833 / (sped_rot*rot_step*2))-1); // number of pulses at each step

Stepper motor(rot_step, ain, amin, bin, bmin);

int sc = 0; // sc = step_count


void setup() {
  pinMode(ain, OUTPUT);  //
  pinMode(amin, OUTPUT);  //
  pinMode(bin, OUTPUT);  //
  pinMode(bmin, OUTPUT);  //
  Serial.begin(9600);
  Serial.println(pulses);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int i = 0; i < rot_step; i++)
  {
    int temp1 = sc % 4;
    int temp2 = (sc + 1) % 4;

    Serial.print("step started : ");
    Serial.println(i + 1);

    first_stepping(temp1, temp2);

    Serial.println("First Pulse");


    for (int j = 0; j < pulses; j++) {
      stepping(temp1, temp2, 15);
      delayMicroseconds(5);
      Serial.print("Pulse check2:");
      Serial.println(j);
    }

    Serial.println("Pulse end.");


    if (sc > 4)
    {
      sc = 0;
    }
    else
    {
      sc++;
    }
  }

}


void first_stepping(int a, int b) {
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  delayMicroseconds(5);

  digitalWrite(a, LOW);
  delayMicroseconds(15);
  digitalWrite(b, LOW);
}
void stepping(int a, int b, int t) {
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  delayMicroseconds(t);
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
}
