OneMsTaskTimer_t timerTask_1 = {TIME_VALUE, RefreshScreenISR,0,0}; //always called every 250ms => timerIRS = 1;
byte hero[8] = {
  B00110,
  B00110,
  B00001,
  B11111,
  B10100,
  B00110,
  B01001,
  B10001,
};
byte hero2[8] = {
  B01100,
  B01100,
  B10000,
  B11111,
  B00101,
  B01100,
  B01010,  
};
byte ob[8] = {
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,  
};
byte bomb[8] = {
  B00000,
  B01000,
  B00110,
  B11111,
  B00110,
  B01000,
  B00000,  
};
byte bomb2[8] = {
  B01000,
  B00110,
  B11111,
  B00110,
  B01000,
  B00000,  
  B00000,  
};

XY prevHeroLocation;
XY ObLocation;
XY prevObLocation;


enum ScreenState{Start,Draw,GameOver};
ScreenState state2;

int timerFlag_1 = 0;
int gameOverFlag = 0; //become 1 when gameover
int heroNum,bombNum;


void setupRefreshScreen(){ //be care for the small mistake of labeling function
  Serial.begin(9600);
  lcd.begin(16, 2); 
  lcd.createChar(0,ob);
  lcd.createChar(1,hero);
  lcd.createChar(2, hero2);
  lcd.createChar(3, bomb);
  lcd.createChar(4, bomb2);
  
  ObLocation.x = 15;
  ObLocation.y = 1;
  state2 = Start;
  
  
  OneMsTaskTimer::add(&timerTask_1);
  OneMsTaskTimer::start();
}

void loopRefreshScreen(){
  while(timerFlag_1 == 0 ){delay(10);} 
  
  //printPosition();
  tickFunc2();
  
  Serial.println(" ");
  timerFlag_1 = 0; //set timerFlag = 0 to while()
  delay(10);
}

void tickFunc2(){
    switch(state2){
      case Start:
        //Serial.println("State2: Start");
        ObLocation.x = 15;
        ObLocation.y = 1;
        if(jumpFlag == 1){
          Serial.println("begin drawing");
          state2 = Draw;
          jumpFlag = 0;
        }
        //state2 = Draw;
      break;
      case Draw:
        //Serial.println("State2: Draw");
        /*if(ObLocation.x == HeroLocation.x && ObLocation.y == HeroLocation.y){
          gameOverFlag = 1;
          state2 = GameOver;
        }*/
        checkCollison(); //check if bomb hit ob
        displayScore();
      break;
      case GameOver:
        //Serial.println("State: GameOver");
        if(gameOverFlag == 0){                  
          state2 = Start;
          }
      break;
    }
  //State Actions
    switch(state2){
      case Start:
      break;
      case Draw:
        //Serial.println("Action: Draw");
        drawNewHero();
        drawNewOb();
        if(bombFlag == 1){
          drawNewBomb();
        }
      break;
      case GameOver:
        //Serial.println("Action: GameOver");
        displayGameOver();
      break;
    }
}
//==Hero draw
void drawNewHero(){
  if(count_cycle%2 == 0){heroNum = 1;}
  else{heroNum = 2;}
  lcd.setCursor(prevHeroLocation.x, prevHeroLocation.y);
  lcd.print(" ");
  lcd.setCursor(HeroLocation.x, HeroLocation.y);
  lcd.write(byte(heroNum));
  prevHeroLocation = HeroLocation;
};
//==Obstacle draw
void drawNewOb(){
  //Serial.print("newObst: ");
  //Serial.print(ObLocation.x);
  //Serial.print(" ");
  //Serial.println(ObLocation.y);
  
  lcd.setCursor(prevObLocation.x, prevObLocation.y);
  lcd.print(" "); 
  lcd.setCursor(ObLocation.x, ObLocation.y);
  lcd.write(byte(0));
  prevObLocation = ObLocation;
  ObLocation.x--;
  if(ObLocation.x <= 0){
    countScore++; //add score
    if((countScore % 3 == 0)&&(countScore != 0)){countBomb++;}
    if(countBomb > 4){countBomb = 3;}
    ObLocation.x = 15;
  }
}

//bomb draw
void drawNewBomb(){
  Serial.print("newBomb: ");
  Serial.print(BombLocation.x);
  Serial.print(" ");
  Serial.println(BombLocation.y);

  if(count_cycle%2 == 0){bombNum = 3;}
  else{bombNum = 4;}
  lcd.setCursor(prevBombLocation.x, prevBombLocation.y);
  lcd.print(" ");
  lcd.setCursor(BombLocation.x, BombLocation.y);
  lcd.write(byte(bombNum));
  prevBombLocation = BombLocation;
  BombLocation.x++;
}

void checkCollison(){
  //status: gameover
  if(ObLocation.x == HeroLocation.x && ObLocation.y == HeroLocation.y){
    gameOverFlag = 1;
    state2 = GameOver;
  }
    
  if(ObLocation.x == BombLocation.x && ObLocation.y == BombLocation.y){
    Serial.println("Collision 1");
    ObLocation.x = 15; //reset obstacle
    lcd.setCursor(prevBombLocation.x, prevBombLocation.y);
    lcd.print(" ");     
    BombLocation.x = -1;
    bombFlag = 0;
  }
  else if(ObLocation.x == BombLocation.x-1 && ObLocation.y == BombLocation.y){
    Serial.println("Collision 2");
    ObLocation.x = 15; //reset obstacle
    lcd.setCursor(prevBombLocation.x, prevBombLocation.y);
    lcd.print(" ");     
    BombLocation.x = -1;
    bombFlag = 0;
  }
  else{}
}

void displayScore(){
  //print score
  Serial.print("Score: ");
  Serial.println(countScore);
  lcd.setCursor(0,0);
  lcd.print("                ");  
  lcd.setCursor(0,0);
  lcd.print("Pts: ");
  lcd.setCursor(5,0);
  lcd.print(countScore);
  
  //print bomb stock
  Serial.print("countBomb: ");
  Serial.println(countBomb);
  lcd.setCursor(13,0);
  lcd.write(byte(3));  
  lcd.setCursor(14,0);
  lcd.print("x");  
  lcd.setCursor(15,0);
  lcd.print(countBomb);  
}

void displayGameOver(){
  lcd.setCursor(0,1);
  lcd.print("                ");  //print for lcd monitor
  lcd.setCursor(0,0);
  lcd.print("                ");  //print for lcd monitor
  lcd.setCursor(0,0);
  lcd.print(" ! Game Over !");
  Serial.println("GameOver");
}
void RefreshScreenISR(){
  timerFlag_1 = 1;
}
