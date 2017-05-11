/*
  Read the comments to see the config of the RC Car please
*/

void Forward(int velocity);
void Reverse(int velocity);
void Left(int velocity);
void Right(int velocity);
void Stop();
 
char command = 'S';
char prevCommand = 'A';
int velocity = 0;   
unsigned long timer0 = 2000;  //Stores the time (in millis since execution started) 
unsigned long timer1 = 0;  //Stores the time when the last command was received from the phone
int RPWM_Output = 5; // Arduino PWM output pin 5; connect to pin 1 (RPWM)
int LPWM_Output = 6; // Arduino PWM output pin 6; connect to pin 2 (LPWM)

void setup() 
{       
  Serial.begin(9600);
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);
}

void loop(){
  if(Serial.available() > 0){ 
    timer1 = millis();   
    prevCommand = command;
    command = Serial.read(); 
    //Change pin mode only if new command is different from previous.   
    if(command!=prevCommand){
      //Serial.println(command);
      switch(command){
      case 'F':  
        Forward(velocity);
        break;
      case 'B':  
        Reverse(velocity);
        break;
      case 'L':  
        Left(velocity);
        break;
      case 'R':
        Right(velocity);  
        break;
      case 'S':  
        Stop();
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
  else{
    timer0 = millis();  //Get the current time (millis since execution started).
    //Check if it has been 500ms since we received last command.
    if((timer0 - timer1)>500){  
      //More tan 500ms have passed since last command received, car is out of range.
      Stop();
    }
  }  
}


void Forward(int velocity){
  int rvelocity = 0;
  rvelocity = rvelocity - velocity;
  analogWrite(RPWM_Output, velocity);
  analogWrite(LPWM_Output, rvelocity);
}


void Reverse(int velocity){
  int rvelocity = 0;
  rvelocity = rvelocity - velocity;
  analogWrite(RPWM_Output, rvelocity);
  analogWrite(LPWM_Output, velocity);
}

void Left(int velocity){
  analogWrite(RPWM_Output, velocity);
  analogWrite(LPWM_Output, velocity);
}

void Right(int velocity){
  int rvelocity = 0;
  rvelocity = rvelocity - velocity;
  analogWrite(RPWM_Output, rvelocity);
  analogWrite(LPWM_Output, velocity);
}

void Stop(){
  analogWrite(RPWM_Output, 0);
  analogWrite(LPWM_Output, 0);
}
