#include "pitches.h"

//Pin connected to ST_CP of 74HC595
int rowLatchPin = 8;
int colLatchPin = 7;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int rowPin = 11;
int colPin = 10;

int playerLeft = 3;
int playerRight = 4;
bool pLeft = false;
bool pRight = false;

int enemyLeft = 5;
int enemyRight = 6;
bool eLeft = false;
bool eRight = false;

int speaker = 2;

int enablePin = 9;

int width = 5;
int height = 14;

bool state[5][14];
int counter = 0;

int playerX, playerY = 0;

int enemyX = 0;
int enemyY = 13;

int ballX = 7;
int ballY = 2;
int velocityX = 1;
int velocityY = 1;

int playerScore = 0;
int enemyScore = 0;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int winning[] = {
  NOTE_E4,NOTE_E4,NOTE_E4,NOTE_C4,NOTE_E4 ,NOTE_G4,NOTE_G3
};

int winningDurations[] = {
  8,4,4,8,4,2,2,
};

int winning2[] = {
  NOTE_D4,NOTE_B3,NOTE_D4,NOTE_B3,NOTE_E4 ,NOTE_B3,NOTE_A4 ,NOTE_B3, NOTE_D4 ,NOTE_B3, NOTE_C4, NOTE_F4,
};

int winningDurations2[] = {
  4,8,4,8,4,8,4,8,4,8,8,3
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(rowLatchPin, OUTPUT);
  pinMode(colLatchPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(rowPin, OUTPUT);
  pinMode(colPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(playerLeft, INPUT);
  pinMode(playerRight, INPUT);
  pinMode(enemyLeft, INPUT);
  pinMode(enemyRight, INPUT);
  digitalWrite(enablePin, 0);

  PlayTune(melody, noteDurations, 8);
}

void loop() {
  state[playerX][playerY] = false;
  state[enemyX][enemyY] = false;
  state[ballX][ballY] = false;

  if(digitalRead(playerLeft) == 1 && pLeft == false) {
      playerX--;
      if(playerX < 0) {
        playerX = 0;
      }
      pLeft = true;
    } else if(digitalRead(playerLeft) == 0) {
      pLeft = false;
    }
    if(digitalRead(playerRight) == 1 && pRight == false ) {
      playerX++;
      if(playerX >= width) {
        playerX = width - 1;
      }
      pRight = true;
    } else if(digitalRead(playerRight) == 0) {
      pRight = false;
    }

  if(digitalRead(enemyLeft) == 1 && eLeft == false) {
      enemyX--;
      if(enemyX < 0) {
        enemyX = 0;
      }
      eLeft = true;
    } else if(digitalRead(enemyLeft) == 0) {
      eLeft = false;
    }
    if(digitalRead(enemyRight) == 1 && eRight == false ) {
      enemyX++;
      if(enemyX >= width) {
        enemyX = width - 1;
      }
      eRight = true;
    } else if(digitalRead(enemyRight) == 0) {
      eRight = false;
    }
  
  //Movement code
  int duration = max((100 / (playerScore + enemyScore + 1)), 20) ; 
  if(counter % duration == 0) {
    if( (ballX == playerX && ballY == playerY) || (ballX == enemyX && ballY == enemyY) )
    {
      velocityY *= -1;
      //ballY += velocityY;
      tone(speaker, NOTE_C5 , duration);
    }
    ballX += velocityX;
    ballY += velocityY;
    if(ballY >= height){
      ballY = height - 2;
      velocityY *= -1;
      playerScore++;
      tone(speaker, NOTE_G4 , duration);
      Score(true);
      
    } else if(ballY < 0){
      ballY = 1;
      velocityY *= -1;
      enemyScore++;
      tone(speaker, NOTE_A3 , duration);
      Score(false);
    }
    if(ballX >= width){
      ballX = width - 2;
      velocityX *= -1;
      tone(speaker, NOTE_C4 , duration);
    } else if(ballX < 0){
      ballX = 1;
      velocityX *= -1;
      tone(speaker, NOTE_E4 , duration); 
    }
  }

  if(playerScore > 6) {
    PlayTune(winning2, winningDurations2, 12);
    playerScore = 0;
    enemyScore = 0;
    
  }
  else if(enemyScore > 6){
    PlayTune(winning, winningDurations, 7);
    playerScore = 0;
    enemyScore = 0;
  }
  
  //Re adjust the state with the updated positions
  state[playerX][playerY] = true;
  state[enemyX][enemyY] = true;
  state[ballX][ballY] = true;
  printState();
  counter++;
}

void PlayTune(int melody[], int noteDurations[], int totalNotes)
{
 for (int thisNote = 0; thisNote < totalNotes; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(speaker, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    int ping = round(map(melody[thisNote],180, 450, 0, height));
    turnOnLed(2, ping);
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speaker);
  }
}

void printState() {
  for(int x = 0; x < width; x++)
  {
    for(int y = 0; y < height; y++)
    {
      if(state[x][y] == true)
      {
         turnOnLed(x,y);
      }
    }
  }
}

void Score(bool bottom){
  
  scoreAnimation(bottom);
  delay(200);
  clearRegisters();
  delay(100);
  scoreAnimation(bottom);
  delay(400);
  clearRegisters();
  delay(100);
  scoreAnimation(bottom);
  delay(200);
}

void scoreAnimation(bool bottom)
{
  clearRegisters();
  digitalWrite(enablePin, 1);
  for(int i = 0; i < width; i += 2) {
      digitalWrite(colPin, 1);
      pulseClock(colLatchPin);
      digitalWrite(colPin, 0);
      pulseClock(colLatchPin);
  }
  
  if(bottom == true) {
   for(int i = 0; i <= playerScore; i++) {
    digitalWrite(rowPin, 1);
    pulseClock(rowLatchPin);
   }
   for(int i = 0; i <=  height - playerScore; i++) {
    digitalWrite(rowPin, 0);
    pulseClock(rowLatchPin);
   }
  }
  else{ 
    for(int i = 0; i <= enemyScore; i++) {
      digitalWrite(rowPin, 1);
      pulseClock(rowLatchPin);
    }
  }
  digitalWrite(enablePin, 0);
}

void turnOnLed(int x,int y)
{
  //Clear the registers first.
  clearRegisters();
  //Get the value ready to go
  digitalWrite(colPin, 1);
  digitalWrite(enablePin, 1);
  pulseClock(colLatchPin);
  //Now that the value will be in the first position, move it to the correct position
  for(int i = 0; i < x; i++)
  {
    digitalWrite(colPin, 0);
    pulseClock(colLatchPin);
  }

  //Do the same thing for the rows
  digitalWrite(rowPin, 1);
  pulseClock(rowLatchPin);
  //Now that the value will be in the first position, move it to the correct position
  for(int i = 0; i <= y; i++) 
  {
    digitalWrite(rowPin, 0);
    pulseClock(rowLatchPin);
  }
  digitalWrite(enablePin, 0);
  
}

void pulseClock()
{
  digitalWrite(rowLatchPin, 0);
  digitalWrite(colLatchPin, 0);
  digitalWrite(clockPin, 0);
  //delay(1);
  digitalWrite(clockPin, 1);
  digitalWrite(rowLatchPin, 1);
  digitalWrite(colLatchPin, 1);
}

void pulseClock(int latchPin)
{
  digitalWrite(latchPin, 0);
  digitalWrite(clockPin, 0);
  //delay(1);
  digitalWrite(clockPin, 1);
  digitalWrite(latchPin, 1);
}

void pulseClock(int latchPin, int otherLatchPin)
{
  digitalWrite(latchPin, 0);
  digitalWrite(otherLatchPin, 0);
  digitalWrite(clockPin, 0);
  //delay(1);
  digitalWrite(clockPin, 1);
  digitalWrite(latchPin, 0);
  digitalWrite(otherLatchPin, 1);
}

void clearRegisters() 
{
  digitalWrite(enablePin, 1);
  for(int i  = 0; i <= height; i++) {
    digitalWrite(rowPin, 0);
    digitalWrite(colPin, 0);
    pulseClock();
  }
  digitalWrite(enablePin, 0);
}


