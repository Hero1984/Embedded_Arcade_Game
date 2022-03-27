//control player's action, bomb and obstacle behavior.

#include "OneMsTaskTimer.h" //already in system library, ""=> user defined 

enum ActionState{GameStart, Wait, MoveForward, MoveBackward, Jump};
ActionState state;

OneMsTaskTimer_t timerTask = {TIME_VALUE, PlayerActionTimerISR,0,0}; //always called every 250ms => timerIRS = 1;
typedef struct xy_struct{
  int x;  
  int y;
}XY; 
XY HeroLocation;

//mile#4
XY BombLocation;
XY prevBombLocation;

//define pins
int selectPin = P4_2; //use button instead 
int yPin = P4_4;


//flags
int jumpFlag = 0; 
int forwardFlag = 0;
int backwardFlag = 0;
int timerFlag = 0;


//threshold
int leftThreshold = 220; 
int rightThreshold_1 = 450;
int rightThreshold_2= 550;

//variables 
int yPinNum = 0;
int count = 0;
int count_cycle = 0;
int countScore = 0;
int countBomb = 0;

//=======================
int bombPin = P4_5; //mile#4 connected to xPin on button
int bombValue = 0;
int bombFlag = 0;
//=======================

void setupPlayerActions(){
  Serial.begin(9600);
    
//  pinMode(selectPin, INPUT);
//  pinMode(bombPin, INPUT);
  pinMode(selectPin, INPUT_PULLUP);
  //pinMode(bombPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(selectPin), jumpISR, RISING); //interrupt every time button pressed   
  //attachInterrupt(digitalPinToInterrupt(bombPin), bombISR, RISING);

  
  state = GameStart; //set to 0
  jumpFlag = 0; //press jump button to start
  
  HeroLocation.x = 0;
  HeroLocation.y = 1;
  
  OneMsTaskTimer::add(&timerTask);
  OneMsTaskTimer::start();
}

void loopPlayerActions(){
  while(timerFlag == 0 ){delay(10);}
  count_cycle++;
  Serial.print("CC: ");
  Serial.println(count_cycle);
    
  readSensor();
  tickFunc();
  
  //printPosition(); //comment out later

  timerFlag = 0; //set timerFlag = 0 to while()
  delay(10); 
}


//track statemachine
void tickFunc(){
  //right:     rightTh_1(450) < num < rightTh_2(550)
  if(rightThreshold_1 < yPinNum && yPinNum < rightThreshold_2){
    forwardFlag = 1;
  }
  else if(yPinNum < leftThreshold){
    backwardFlag = 1;
  }
  else{
  }
  
  //stick value used for bomb
  if(bombValue >= 950 && countBomb > 0){
    bombISR();  
    countBomb--;
  }
  
  //state transit
    switch(state){
    case GameStart:
      //Serial.println("Transit: Start to Wait ???????");
      HeroLocation.x = 0;
      HeroLocation.y = 1;
     
      Serial.println("Press to start!");
      lcd.setCursor(0, 0);
      lcd.print("Press to Start!"); //mile#4
      if(jumpFlag == 1){
        Serial.println("Button Pressed");
        lcd.setCursor(0, 0);
        lcd.print("                "); 
        state = Wait;
        //jumpFlag = 0;
      }
      break;  
    case Wait:
      //Serial.println("State: Wait");      
      if(forwardFlag == 1 && jumpFlag == 0){
        //Serial.println("Transit: to Forward");
        state = MoveForward;      
      }
      else if(backwardFlag == 1 && jumpFlag == 0){
        //Serial.println("Transit: Wait to Backward");      
        state = MoveBackward;
      }
      else if(jumpFlag != 0){
        //Serial.println("Transit: to Jump");
        count = 0;
        state = Jump;
      }
      else{
      }
      break;
    case MoveForward:
      //Serial.println("State: Forward");      
      //state = Wait;
      break;
    case MoveBackward: 
      //Serial.println("State: Backward");
      //state = Wait;
      break;
    case Jump:
      //Serial.println("State: Jump");      
      break;
    default:
      state = Wait;
  }

    //state action
    switch(state){
    case GameStart:
      break;  
    case Wait:
      //Serial.println("Action: Wait");
      break;
    case MoveForward:
      if(HeroLocation.x < 15 && HeroLocation.x > 0){
        HeroLocation.x++;
      }
      else if(HeroLocation.x == 0){
        HeroLocation.x++;  
      }
      forwardFlag = 0;
      state = Wait; 
      break;
    case MoveBackward:
      if(HeroLocation.x < 15 && HeroLocation.x > 0){
        HeroLocation.x--;
      }
      else if(HeroLocation.x == 15){
        HeroLocation.x--;  
      }
      backwardFlag = 0;
      state = Wait; 
      break;
    case Jump:
      if(count < 2){
        HeroLocation.y = 0;
        count++;
      }
      else if(count >= 2){
        HeroLocation.y = 1;
        jumpFlag = 0;
        state = Wait;
      }
      break;
  }   
}
//detect joystick 
void readSensor(){
  yPinNum = analogRead(yPin);  
  bombValue = analogRead(bombPin);
  Serial.print("yPin bombPin: ");
  Serial.print(yPinNum);
  Serial.print(" ");
  Serial.println(bombValue);
}

void jumpISR(){
  jumpFlag = 1;
  Serial.println("jumpISR");
}
void PlayerActionTimerISR(){
  timerFlag = 1;
}
void printPosition(){
  Serial.print("NewHero:  ");
  Serial.print(HeroLocation.x);
  Serial.print(" ");
  Serial.println(HeroLocation.y);
}


//mileSton#4
void bombISR(){
  bombFlag = 1;
  Serial.println("bombISR");
  //initialize bombLocation
  BombLocation.x = HeroLocation.x + 1;
  BombLocation.y = HeroLocation.y;
  prevBombLocation = BombLocation;
}

void checkBomb(){
  
}
  
