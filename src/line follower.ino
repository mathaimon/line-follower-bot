#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 /*128 width of OLED in pixels*/
#define SCREEN_HEIGHT 64 /*64 height of OLED in pixels*/

int leftIR = 7;                       //Left IR sensor for line following
int middleIR = 8;                     //Middle IR sensor for line following
int rightIR = 9;                      //Right IR sensor for line following
 
int reedSwitch =12 ;

int leftMotorForward = 5;              //left motor rotates forward
int leftMotorBackward = 6;            //left motor rotates backward
// int leftMotorEN = 0;                  //left motor enable pin
int rightMotorForward = 10;             //right motor rotates forward
int rightMotorBackward = 11;           //right motor rotates backward
// int rightMotorEN = 4;                  //right motor enable pin

const int trigPin = 15;
const int echoPin = 14;

long duration;
int distance;
int minThreshold = 20;
int maxThreshold = 30;

int totalSlots = 3;       
int allotedSlot = 1;
int currentSlot = 0;
int lastStatereedSwitch = 1;
int currentStatereedSwitch = 1;
//delay for reedSwitch sensor, so it doesn't take multiple readings from single magnet
unsigned long delay_reedSwitch = 1000;
unsigned long previousMillis_reedSwitch = 0;
unsigned long currentMillis_reedSwitch = 0;
//delay for parking the vehicle in the alloted slot
// unsigned long delay_Parking = 10000;
// unsigned long previousMillis_Parking = 0;
// unsigned long currentMillis_Parking = 0;

int parked = 0;                        //whether the vehicle is parked or not in the alloted slot (0 - not parked and 1 - parked)
int reversed = 0;
// int leftIRON =0;
// int middleIRON =0;
// int rightIRON =0;


int leftIRVal;
int middleIRVal;
int rightIRVal;

int lowSpeed= 60;
int medSpeed= 100;
int highSpeed=255;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  pinMode(leftIR,INPUT);
  pinMode(rightIR,INPUT); 
  pinMode(middleIR,INPUT);
  pinMode(reedSwitch,INPUT_PULLUP);
    
  pinMode(leftMotorForward,OUTPUT);
  pinMode(leftMotorBackward,OUTPUT);
  // pinMode(leftMotorEN,OUTPUT);

  pinMode(rightMotorForward,OUTPUT);
  pinMode(rightMotorBackward,OUTPUT);
  // pinMode(rightMotorEN,OUTPUT);
}

int IRSensorsOnLine(){
  if (digitalRead(leftIR)==1 && digitalRead(rightIR)==1 && digitalRead(middleIR)==1){
    return 1;
  }
  else
    return 0;
}

int IRSensorsNotOnLine(){
  if (digitalRead(leftIR)==0 && digitalRead(rightIR)==0 && digitalRead(middleIR)==0){
    return 1;
  }
  else
    return 0;
}

// int IRSensorON(){
//   if (digitalRead(leftIR)==1){
//     leftIRON=1;
//   }
//   if (digitalRead(middleIR)==1){
//     middleIRON=1;
//   }
//   if (digitalRead(rightIR)==1){
//     rightIRON=1;
//   }
// }

void IR3LineFollowing(){
  Serial.print("3 IRLine");
  display.setCursor(0, 30);
  display.print("3 IRLine");

  //"IR sensor ON"    or    "Detects object"      or    "not on line"   then the output is     "0"    or    "LOW"
  //"IR sensor OFF"   or    "Detects no object"   or    "on line"       then the output is     "1"    or    "HIGH"

  leftIRVal = digitalRead(leftIR);
  rightIRVal = digitalRead(rightIR);
  middleIRVal = digitalRead(middleIR);
  // Serial.print(leftIRVal);
  // Serial.print('\t');
  // Serial.print(rightIRVal);
  // Serial.print('\t');
  // Serial.print(middleIRVal);
  // Serial.println('\t');

  if (leftIRVal==LOW && middleIRVal==LOW && rightIRVal==LOW){         //When no line is detected the bot stops
    digitalWrite(leftMotorForward,LOW);
    digitalWrite(leftMotorBackward,LOW);
    // digitalWrite(leftMotorEN,LOW);

    digitalWrite(rightMotorForward,LOW);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,LOW);
  }

  else if (leftIRVal==LOW && middleIRVal==LOW && rightIRVal==LOW){         //When every ir sensor detects line the bot stops
    digitalWrite(leftMotorForward,LOW);
    digitalWrite(leftMotorBackward,LOW);
    // digitalWrite(leftMotorEN,LOW);

    digitalWrite(rightMotorForward,LOW);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,LOW);
  }

  else if (leftIRVal==HIGH  && middleIRVal==HIGH && rightIRVal==LOW){ //When the left and middle sensor is on line
    digitalWrite(leftMotorForward,LOW);                               //the left motor is stopped and right motor
    digitalWrite(leftMotorBackward,LOW);                              //runs forward
    // digitalWrite(leftMotorEN,LOW);

    analogWrite(rightMotorForward,medSpeed);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,medSpeed);

  }
  else if (leftIRVal==HIGH && middleIRVal==LOW && rightIRVal==LOW){   //When the middle and right sensor is 'not' on line
    digitalWrite(leftMotorForward,LOW);                               //but the left sensor is on line then,
    analogWrite(leftMotorBackward,medSpeed);                             //left motor runs backward and right motor runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,medSpeed);

    analogWrite(rightMotorForward,medSpeed);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,medSpeed);
  }
  else if(leftIRVal==LOW  && middleIRVal==HIGH && rightIRVal==HIGH){  //When the middle and right sensor is on line
    analogWrite(leftMotorForward,medSpeed);                              //the right motor is stopped and left motor
    digitalWrite(leftMotorBackward,LOW);                              // runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,medSpeed);

    digitalWrite(rightMotorForward,LOW);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(leftMotorEN,LOW);
  }
  else if(leftIRVal==LOW  && middleIRVal==LOW && rightIRVal==HIGH){   //When the left and middle sensor is 'not' on line
    analogWrite(leftMotorForward,medSpeed);                              //but the right sensor is on line then,
    digitalWrite(leftMotorBackward,LOW);                              //left motor runs forward and right motor runs backward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,medSpeed);

    digitalWrite(rightMotorForward,LOW);
    analogWrite(rightMotorBackward,medSpeed);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,medSpeed);
  }
  else if(leftIRVal==LOW && middleIRVal==HIGH && rightIRVal==LOW){    //when both the left and right sensors are not on line
    analogWrite(leftMotorForward,medSpeed);                              // and middle sensor is on line then the bot goes
    digitalWrite(leftMotorBackward,LOW);                              // straight i.e. both the motors runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,medSpeed);

    analogWrite(rightMotorForward,medSpeed);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,medSpeed);
  }

}

void rightIRLineFollowing(){
  Serial.print("RIGHTIRLine");
  display.setCursor(0, 30);
  display.print("RIGHTIRLine");
  //"IR sensor ON"    or    "Detects object"      or    "not on line"   then the output is     "0"    or    "LOW"
  //"IR sensor OFF"   or    "Detects no object"   or    "on line"       then the output is     "1"    or    "HIGH"

  leftIRVal = digitalRead(leftIR);
  rightIRVal = digitalRead(rightIR);
  middleIRVal = digitalRead(middleIR);

  if (middleIRVal==HIGH && rightIRVal==LOW){                          //when the right sensor is not on line and
    analogWrite(leftMotorForward,medSpeed);                              // the middle sensor is on line then the bot goes
    digitalWrite(leftMotorBackward,LOW);                              // straight i.e. both the motors runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    analogWrite(rightMotorForward,medSpeed);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,lowSpeed);
  }
  else if (middleIRVal==LOW && rightIRVal==LOW){                      //When the middle and right sensors are 'not' on line
    digitalWrite(leftMotorForward,LOW);                           //left motor is stopped and right motor runs forward
    digitalWrite(leftMotorBackward,LOW);     
    // digitalWrite(leftMotorEN,LOW);

    analogWrite(rightMotorForward,medSpeed);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,lowSpeed);
  }
  else if(middleIRVal==HIGH && rightIRVal==HIGH){                     //When the middle and right sensor is on line
    analogWrite(leftMotorForward,medSpeed);                              //the right motor is stopped and left motor
    digitalWrite(leftMotorBackward,LOW);                              // runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    analogWrite(rightMotorForward,LOW);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(leftMotorEN,LOW);
  }
  else if(middleIRVal==LOW && rightIRVal==HIGH){                      //When the middle sensor is 'not' on line
    analogWrite(leftMotorForward,medSpeed);                              //but the right sensor is on line then,
    digitalWrite(leftMotorBackward,LOW);                              //left motor runs forward and right motor runs backward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    digitalWrite(rightMotorForward,LOW);
    analogWrite(rightMotorBackward,medSpeed);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,LOW);
  }
  
}

void leftIRLineFollowing(){
  Serial.print("LEFTIRLine");
  display.setCursor(0, 30);
  display.print("LEFTIRLine");
  //"IR sensor ON"    or    "Detects object"      or    "not on line"   then the output is     "0"    or    "LOW"
  //"IR sensor OFF"   or    "Detects no object"   or    "on line"       then the output is     "1"    or    "HIGH"

  leftIRVal = digitalRead(leftIR);
  rightIRVal = digitalRead(rightIR);
  middleIRVal = digitalRead(middleIR);

  if (leftIRVal==HIGH  && middleIRVal==HIGH){                       //When the left and middle sensor is on line
    digitalWrite(leftMotorForward,LOW);                           // the left motor is stopped and right motor
    digitalWrite(leftMotorBackward,LOW);                          // runs forward
    // digitalWrite(leftMotorEN,LOW);

    analogWrite(rightMotorForward,medSpeed);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,lowSpeed);
  }
  else if (leftIRVal==HIGH && middleIRVal==LOW){                   //When the middleis 'not' on line and
    digitalWrite(leftMotorForward,LOW);                            //the left sensor is on line then,
    analogWrite(leftMotorBackward,medSpeed);                          //left motor runs backward and right motor runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    analogWrite(rightMotorForward,medSpeed);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,lowSpeed);
  }
  else if(leftIRVal==LOW  && middleIRVal==LOW){                    //When the left and middle sensor is 'not' on line
    analogWrite(leftMotorForward,medSpeed);                           //left motor runs forward and right motor runs backward
    digitalWrite(leftMotorBackward,LOW);          
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    digitalWrite(rightMotorForward,LOW);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,LOW);
    // analogWrite(rightMotorEN,lowSpeed);
  }
  else if(leftIRVal==LOW && middleIRVal==HIGH){                    //when both the left not on line and 
    analogWrite(leftMotorForward,medSpeed);                           //middle sensor is on line then the bot goes
    digitalWrite(leftMotorBackward,LOW);                           // straight i.e. both the motors runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    analogWrite(rightMotorForward,medSpeed);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,lowSpeed);
  }
}

void reverseLineFollowing(){
  Serial.print("reverseLINE");
  display.setCursor(0, 30);
  display.print("reverseLINE");
  //"IR sensor ON"    or    "Detects object"      or    "not on line"   then the output is     "0"    or    "LOW"
  //"IR sensor OFF"   or    "Detects no object"   or    "on line"       then the output is     "1"    or    "HIGH"

  leftIRVal = digitalRead(leftIR);
  rightIRVal = digitalRead(rightIR);
  middleIRVal = digitalRead(middleIR);

  if (leftIRVal==LOW && middleIRVal==LOW && rightIRVal==LOW){         //When no line is detected the bot stops
    digitalWrite(leftMotorForward,LOW);
    digitalWrite(leftMotorBackward,LOW);
    // digitalWrite(leftMotorEN,LOW);

    digitalWrite(rightMotorForward,LOW);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,LOW);
  }

  else if (leftIRVal==HIGH  && middleIRVal==HIGH && rightIRVal==LOW){ //When the left and middle sensor is on line
    digitalWrite(leftMotorForward,LOW);                               //the left motor is stopped and right motor
    analogWrite(leftMotorBackward,medSpeed);                              //runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    digitalWrite(rightMotorForward,LOW);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,LOW);
  }
  else if (leftIRVal==HIGH && middleIRVal==LOW && rightIRVal==LOW){   //When the middle and right sensor is 'not' on line
    digitalWrite(leftMotorForward,LOW);                               //the left motor is stopped and right motor
    analogWrite(leftMotorBackward,medSpeed);                              //runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    digitalWrite(rightMotorForward,LOW);
    digitalWrite(rightMotorBackward,LOW);
    // digitalWrite(rightMotorEN,LOW);
  }
  else if(leftIRVal==LOW  && middleIRVal==HIGH && rightIRVal==HIGH){  //When the middle and right sensor is on line
    digitalWrite(leftMotorForward,LOW);                              //the right motor is stopped and left motor
    digitalWrite(leftMotorBackward,LOW);                              // runs forward
    // digitalWrite(leftMotorEN,LOW);

    digitalWrite(rightMotorForward,LOW);
    analogWrite(rightMotorBackward,medSpeed);
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(rightMotorEN,lowSpeed);
  }
  else if(leftIRVal==LOW  && middleIRVal==LOW && rightIRVal==HIGH){   //When the left and middle sensor is 'not' on line
    digitalWrite(leftMotorForward,LOW);                              //but the right sensor is on line then,
    digitalWrite(leftMotorBackward,LOW);                              //left motor runs forward and right motor runs backward
    // digitalWrite(leftMotorEN,LOW);

    digitalWrite(rightMotorForward,LOW);
    analogWrite(rightMotorBackward,medSpeed);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,lowSpeed);
  }
  else if(leftIRVal==LOW && middleIRVal==HIGH && rightIRVal==LOW){    //when both the left and right sensors are not on line
    digitalWrite(leftMotorForward,LOW);                              // and middle sensor is on line then the bot goes
    analogWrite(leftMotorBackward,medSpeed);                              // straight i.e. both the motors runs forward
    // digitalWrite(leftMotorEN,HIGH);
    // analogWrite(leftMotorEN,lowSpeed);

    digitalWrite(rightMotorForward,LOW);
    analogWrite(rightMotorBackward,medSpeed);
    // digitalWrite(rightMotorEN,HIGH);
    // analogWrite(rightMotorEN,lowSpeed);
  }
}

void botForward(){
  Serial.print("botForward");
  display.setCursor(0, 30);
  display.print("botForward");

  analogWrite(leftMotorForward,lowSpeed);
  digitalWrite(leftMotorBackward,LOW);
  //  digitalWrite(leftMotorEN,LOW);
  // analogWrite(leftMotorEN,lowSpeed);

  analogWrite(rightMotorForward,lowSpeed);
  digitalWrite(rightMotorBackward,LOW);
  // digitalWrite(rightMotorEN,LOW);
  // analogWrite(rightMotorEN,lowSpeed);
}

void botBackward(){
  Serial.print("botBackward");
  display.setCursor(0, 30);
  display.print("botBackward");
  digitalWrite(leftMotorForward,LOW);
  analogWrite(leftMotorBackward,lowSpeed);
  //  digitalWrite(leftMotorEN,LOW);
  // analogWrite(leftMotorEN,lowSpeed);

  digitalWrite(rightMotorForward,LOW);
  analogWrite(rightMotorBackward,lowSpeed);
  // digitalWrite(rightMotorEN,LOW);
  // analogWrite(rightMotorEN,lowSpeed);
}

void botStop(){
  Serial.print("Stopped");
  display.setCursor(0, 30);
  display.print("Stopped");

  digitalWrite(leftMotorForward,LOW);
  digitalWrite(leftMotorBackward,LOW);
  //  digitalWrite(leftMotorEN,LOW);
  // analogWrite(leftMotorEN,0);

  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  // digitalWrite(rightMotorEN,LOW);
  // analogWrite(rightMotorEN,0);
}

void loop(){
  display.clearDisplay();  /*Clear display*/
  display.setTextSize(2);  /*OLED screen text size defined*/
  display.setTextColor(WHITE); /*OLED screen text color*/

  currentMillis_reedSwitch= millis();
  currentStatereedSwitch = digitalRead(reedSwitch);

  if(currentStatereedSwitch != lastStatereedSwitch){
    lastStatereedSwitch=currentStatereedSwitch;
    if (currentStatereedSwitch == 0 && currentMillis_reedSwitch-previousMillis_reedSwitch >= delay_reedSwitch){
    
      previousMillis_reedSwitch=currentMillis_reedSwitch;
          
      if(currentSlot>=totalSlots){
        currentSlot=1;
      }
      else{
        currentSlot++;
      }   
    } 
  }
  
  if (currentSlot==allotedSlot){
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else
    digitalWrite(LED_BUILTIN,LOW);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  
  if(distance<=maxThreshold && distance>=minThreshold){
    botStop();
  }
  else if(distance<=minThreshold){
    reverseLineFollowing();
  }
  else if (currentSlot==0 && distance>=maxThreshold){
    IR3LineFollowing();
  }
  else if(currentSlot==allotedSlot && IRSensorsNotOnLine()==0 && parked==0 && reversed==0){
    leftIRLineFollowing();    
  }
  else if (currentSlot==allotedSlot && IRSensorsNotOnLine()==1 && parked==0 && reversed==0){
    parked=1;
    botStop();
    delay(10000);
  }  
  else if(currentSlot==allotedSlot && IRSensorsNotOnLine()==1 && parked==1){
    parked=0;
    reversed=1;
    do{
      Serial.print("Back ");
      botBackward();
    }while(IRSensorsNotOnLine()==1); 
    do{
      // reverseLineFollowing();
      botBackward();
    }while(IRSensorsOnLine()==0 && IRSensorsNotOnLine()==0);
    do{
      botForward();
    }while(IRSensorsNotOnLine()==1);
        
  }
  else if(currentSlot!=allotedSlot && parked==0){
    rightIRLineFollowing();
    reversed=0;
  }
  else if(currentSlot==allotedSlot && (IRSensorsOnLine()==0 || IRSensorsNotOnLine()==0) && reversed==1){
    Serial.print("reverse ");
    rightIRLineFollowing();
  }
  
  else{
    Serial.print("else ");
    botStop();
  }



  Serial.print("  CS:");
  Serial.print(currentSlot);
  Serial.print("  RS:");
  Serial.print(digitalRead(reedSwitch));
  Serial.print("  Dist:");
  Serial.println(distance);

  display.setTextSize(1);  /*OLED screen text size defined*/

  display.setCursor(0, 0);
  display.print("  CS:");
  display.println(currentSlot);
  display.setCursor(0, 10);
  display.print("  RS:");
  display.println(digitalRead(reedSwitch));
  display.setCursor(0, 20);
  display.print("  Dist:");
  display.println(distance);
  
  display.display();
}