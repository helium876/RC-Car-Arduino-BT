/*
 SETUP
 LINK TO APP
 https://play.google.com/store/apps/details?id=braulio.calle.bluetoothRCcontroller

 Connections:
 Bluetooth (e.g HC-05)-> Arduino Mega 2560
 TXD - RX1 (19)
 RXD - TX1 (18)
 VCC - 5V
 GND - GND
 
 BTS7960 -> Arduino Mega 2560
 MotorRight_R_EN - 4
 MotorRight_L_EN - 5
 MotorLeft_R_EN - 8
 MotorLeft_L_EN - 9
 Rpwm1 - 6
 Lpwm1 - 7
 Rpwm2 - 2
 Lpwm2 - 3
 
 ROBOT CONTROL STATES:
 0 - stop_Robot
 1 - turn_Right
 2 - turn_Left
 3 - go_Forward
 4 - go_Backward
 */
#include <math.h>

/*BTS7960 Motor Driver Carrier*/
const int MotorRight_R_EN = 4; // Pin to control the clockwise direction of Right Motor
const int MotorRight_L_EN = 5; // Pin to control the counterclockwise direction of Right Motor 
const int MotorLeft_R_EN = 8; // Pin to control the clockwise direction of Left Motor
const int MotorLeft_L_EN = 9; // Pin to control the counterclockwise direction of Left Motor
const int Rpwm1 = 6; // pwm output - motor A
const int Lpwm1 = 7; // pwm output - motor B
const int Rpwm2 = 2; // pwm output - motor A
const int Lpwm2 = 3; // pwm output - motor B
long pwmLvalue = 255;
long pwmRvalue = 255;
byte pwmChannel;
char command = 'S';
char prevCommand = 'A';
int velocity = 0;   
unsigned long timer0 = 2000;  //Stores the time (in millis since execution started) 
unsigned long timer1 = 0;  //Stores the time when the last command was received from the phone

const char startOfNumberDelimiter = '<';
const char endOfNumberDelimiter = '>';
int robotControlState;
int last_mspeed;

void setup(){
  /*
  For Arduino Mega 2560
  Serial1 RX - pin 19
  Serial1 TX - pin 18
  */
  
  Serial1.begin(9600);//Default Bluetooth Baudrate for HC-05
  Serial.begin(9600);

  //Setup Right Motors
  pinMode(MotorRight_R_EN, OUTPUT); //Initiates Motor Channel A1 pin
  pinMode(MotorRight_L_EN, OUTPUT); //Initiates Motor Channel A2 pin

  //Setup Left Motors
  pinMode(MotorLeft_R_EN, OUTPUT); //Initiates Motor Channel B1 pin
  pinMode(MotorLeft_L_EN, OUTPUT); //Initiates Motor Channel B2 pin
  
  //Setup PWM pins as Outputs
  pinMode(Rpwm1, OUTPUT);
  pinMode(Lpwm1, OUTPUT);
  pinMode(Rpwm2, OUTPUT);
  pinMode(Lpwm2, OUTPUT);
  
  stop_Robot();
}// void setup()

void loop(){
  if(Serial1.available() > 0){ 
    timer1 = millis();   
    prevCommand = command;
    command = Serial1.read(); 
    //Change pin mode only if new command is different from previous.   
    if(command!=prevCommand){
      Serial.println(command);
      switch(command){
      case 'L':  
        go_Forward(velocity);
        break;
      case 'R':  
        go_Backwad(velocity);
        break;
      case 'B':  
        turn_Left(velocity);
        break;
      case 'F':
        turn_Right(velocity);  
        break;
      case 'S':  
        stopRobot(1);
        break; 
      default:  //Get velocity
        if(command=='q'){
          velocity = 255;  //Full velocity
        }
        else{ 
          //Chars '0' - '9' have an integer equivalence of 48 - 57, accordingly.
          if((command >= 48) && (command <= 57)){ 
            //Subtracting 48 changes the range from 48-57 to 0-9.
            //Multiplying by 25 changes the range from 0-9 to 0-225.
            velocity = (command - 48)*25;       
          }
        }
      }
    }
  } 
}

void stop_Robot(){ 
    SetMotors(2); 
    analogWrite(Rpwm1, 0);
    analogWrite(Lpwm1, 0);
    analogWrite(Rpwm2, 0);
    analogWrite(Lpwm2, 0);
    robotControlState = 0;
}

void turn_Right(int mspeed){ 
    SetMotors(1);
    analogWrite(Rpwm1, 0);
    analogWrite(Lpwm1, mspeed);
    analogWrite(Rpwm2, mspeed);
    analogWrite(Lpwm2, 0);
    robotControlState=1;
    last_mspeed=mspeed;
}

void turn_Left(int mspeed){ 
    SetMotors(1);
    analogWrite(Rpwm1, mspeed);
    analogWrite(Lpwm1, 0);
    analogWrite(Rpwm2, 0);
    analogWrite(Lpwm2, mspeed);
    robotControlState=2;
    last_mspeed=mspeed;
}

void go_Forward(int mspeed){ 
    SetMotors(1);
    analogWrite(Rpwm1, mspeed);
    analogWrite(Lpwm1, 0);
    analogWrite(Rpwm2, mspeed);
    analogWrite(Lpwm2, 0);
    robotControlState=3;
    last_mspeed=mspeed;
}

void go_Backwad(int mspeed){
    SetMotors(1);
    analogWrite(Rpwm1, 0);
    analogWrite(Lpwm1, mspeed);
    analogWrite(Rpwm2, 0);
    analogWrite(Lpwm2, mspeed);
    robotControlState=4;
    last_mspeed=mspeed;
}

void stopRobot(int delay_ms){
  SetMotors(2);
  analogWrite(Rpwm1, 0);
  analogWrite(Lpwm1, 0);
  analogWrite(Rpwm2, 0);
  analogWrite(Lpwm2, 0);
  delay(delay_ms);
}

void SetPWM(const long pwm_num, byte pwm_channel){
  if(pwm_channel==1){ 
    analogWrite(Rpwm1, 0);
    analogWrite(Rpwm2, 0);
    analogWrite(Lpwm1, pwm_num);
    analogWrite(Lpwm2, pwm_num);
    pwmRvalue = pwm_num;
  }
  else if(pwm_channel==2){ 
    analogWrite(Lpwm1, 0);
    analogWrite(Lpwm2, 0);
    analogWrite(Rpwm1, pwm_num);
    analogWrite(Rpwm2, pwm_num);
    pwmLvalue = pwm_num;
  }
}

void SetMotors(int controlCase){
  switch(controlCase){
    case 1:
      digitalWrite(MotorRight_R_EN, HIGH);  
      digitalWrite(MotorRight_L_EN, HIGH); 
      digitalWrite(MotorLeft_R_EN, HIGH);  
      digitalWrite(MotorLeft_L_EN, HIGH); 
    break;
    case 2:
      digitalWrite(MotorRight_R_EN, LOW);  
      digitalWrite(MotorRight_L_EN, LOW); 
      digitalWrite(MotorLeft_R_EN, LOW);  
      digitalWrite(MotorLeft_L_EN, LOW); 
    break;
  } 
}
