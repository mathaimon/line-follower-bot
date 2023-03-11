#include <Arduino.h>

int leftIR = 3;    //Left IR sensor line follower
int rightIR = 4;   //Right IR sensor line follower 
int middleIR = 5;  //Middle IR sensor line follower 

int hallEffect = 6;

int leftMotorCW = 7;      //left motor rotates clockwise
int leftMotorCCW = 8;     //left motor rotates anti-clockwise
int leftMotorEN = 9;      //left motor enable pin

int rightMotorCW = 10;    //right motor rotates clockwise
int rightMotorCCW = 11;   //right motor rotates anti-clockwise
int rightMotorEN = 12;    //right motor enable pin

int ir1;
int ir2;
int ir3;
int ir4;

void setup() {
  pinMode(leftIR,INPUT);
  pinMode(rightIR,INPUT); 
  pinMode(middleIR,INPUT);
  pinMode(hallEffect,INPUT);
    
  pinMode(leftMotorCW,OUTPUT);
  pinMode(leftMotorCCW,OUTPUT);
  pinMode(leftMotorEN,OUTPUT);

  pinMode(rightMotorCW,OUTPUT);
  pinMode(rightMotorCCW,OUTPUT);
  pinMode(rightMotorEN,OUTPUT);
}

void loop(){

ir1 = digitalRead(IR1); ///reading the inputs from IR sensor 1
ir2 = digitalRead(IR2); ///readfing the inputs from IR senssor 2
ir3 = digitalRead(IR3);  ///reading the inputs from IR sensor 3
ir4 = digitalRead(IR4);

///adding the statements for performing the functions

if (ir4==LOW)
{
digitalWrite(motor1,LOW);
digitalWrite(motor1b,LOW);
digitalWrite(motor2,LOW);
digitalWrite(motor2b,LOW);
}
else if (ir1==LOW && ir2==LOW && ir3==LOW)
{
digitalWrite(motor1,LOW);
digitalWrite(motor1b,LOW);
digitalWrite(motor2,LOW);
digitalWrite(motor2b,LOW);
}

else if (ir1==HIGH && ir2==LOW && ir3==HIGH)
{
  digitalWrite(motor1,HIGH);
digitalWrite(motor1b,LOW);
digitalWrite(motor2,LOW);
digitalWrite(motor2b,HIGH);
}
else if (ir1==HIGH && ir2==LOW && ir3==LOW)
{
  digitalWrite(motor1,HIGH);
digitalWrite(motor1b,LOW);
digitalWrite(motor2,LOW);
digitalWrite(motor2b,HIGH);
}
else if(ir1==LOW && ir2==HIGH  && ir3==HIGH)
{
digitalWrite(motor1,LOW);
digitalWrite(motor1b,HIGH);
digitalWrite(motor2,HIGH);
digitalWrite(motor2b,LOW);
}
else if(ir1==LOW && ir2==HIGH  && ir3==LOW)
{
digitalWrite(motor1,LOW);
digitalWrite(motor1b,HIGH);
digitalWrite(motor2,HIGH);
digitalWrite(motor2b,LOW);
}
else if(ir1==LOW && ir2==LOW && ir3==HIGH && ir4==HIGH)
{
digitalWrite(motor1,HIGH);
digitalWrite(motor1b,LOW);
digitalWrite(motor2,HIGH);
digitalWrite(motor2b,LOW);
}

}