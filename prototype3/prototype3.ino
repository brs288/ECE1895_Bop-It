#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_ImageReader.h> // Image-reading functions
#include <DFRobotDFPlayerMini.h>  // Audio Library

#define SD_CS   4 // SD card select pin
#define TFT_DC  9 // TFT display/command pin
#define TFT_CS 10 // TFT select pin
#define JSY    A5 // Joystick y
#define JSX    A4 // Joystick x
#define START  A3 // Start button
#define PUNCH  A2 // Punch button
#define KICK   A1 // Kick button
#define BLOCK  A0 // Block button


SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys


Adafruit_ILI9341     tft    = Adafruit_ILI9341(TFT_CS, TFT_DC); // display
DFRobotDFPlayerMini player; // audioplayer

int level = 0;
int timeLimit = 8500;
int timeInitial = timeLimit;
unsigned long startTime;
int targetButton;
int targetPositionX;
int targetPositionY;
bool gameRunning = false;

void setup(void) {

  randomSeed(analogRead(A0));
  // Initialize Pin de
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  tft.begin();          // Initialize screen
  tft.setRotation(1);
  
  // SD card is pretty straightforward, a single call...
  if(!SD.begin(SD_CS, SD_SCK_MHZ(25))) { // ESP32 requires 25 MHz limit
    for(;;); // Fatal error, do not continue
  }

  Serial.begin(9600); // Initialize serial communication

  if (!player.begin(Serial)) {
    while (true); // Stop execution
  }

  // Set volume (0 to 30)
  player.volume(15);
  
  resetGame();  // Start in "Press Start" state
}

void loop() {
    if (!gameRunning) {
        reader.drawBMP("/3012.bmp", tft, 0, 0); // wait screen
        waitForStart(); // Scans for an input and will hold until detected
        gameRunning = true;
        level = 0;
        timeLimit = 8500;
        reader.drawBMP("/6000.bmp", tft, 0, 0); // draw the intro sequence
        reader.drawBMP("/6001.bmp", tft, 0, 0);
        reader.drawBMP("/6002.bmp", tft, 0, 0);
        player.playFolder(3, 1);
        reader.drawBMP("/6003.bmp", tft, 0, 0);
    }
    
    bool result = playLevel(); // each "level" results in a pass or fail

    if (!result) { // Indicates the player failed by timeout or incorrect input
      gameOver();
      delay(2000);
      resetGame();
    }
}

void waitForStart() {
    player.loopFolder(4); // play menu music
    while (true) {
        if (digitalRead(START) == HIGH) {
            player.stop();
            player.volume(30);
            delay(200);  // Debounce delay
            return;
        }
    }
}

void levelUp() {
    if (level == 99) {
        player.playFolder(3, 2); // play victory
        victoryScreen();
        delay(2000);
        resetGame();
    } else {
        level++;
        if (level % 10 == 0) timeLimit = timeInitial * exp(-(1.75 / 100) * level + 1.1) / 3.0;
        delay(500);
    }
}

void resetGame() {
    gameRunning = false;
}

void gameOver() {
    for(int i = 0; i < 3; i++) {
      int index = (3009 + (13 * level) + i);
      String filename = String(index) + ".bmp";
      if (i == 0) player.playFolder(2,1); // play hit sound
      if (i == 2) player.playFolder(3, 3); // play evil laugh
      reader.drawBMP(filename.c_str(), tft, 0, 0);
    }
}


void victoryScreen() {
    reader.drawBMP("/6004.bmp", tft, 0, 0);

}

int getPressedButton() {
    if (digitalRead(PUNCH) == HIGH && digitalRead(KICK) == LOW && digitalRead(BLOCK) == LOW) return 0;
    if (digitalRead(KICK) == HIGH && digitalRead(PUNCH) == LOW && digitalRead(BLOCK) == LOW)  return 1;
    if (digitalRead(BLOCK) == HIGH && digitalRead(PUNCH) == LOW && digitalRead(KICK) == LOW) return 2;
    return -1; // No input detected
}

int getXPosition() {
    if (analogRead(JSX) < 256) return 0; // LEFT
    if (analogRead(JSX) > 768) return 1; // RIGHT
    return 2; // NEUTRAL
}

int getYPosition() {
    if (analogRead(JSY) < 256) return 0; // UP
    if (analogRead(JSY) > 768) return 1; // DOWN
    return 2; // NEUTRAL
}

bool playLevel() {
    targetButton = random(0, 3);    // PUNCH == 0, KICK == 1, BLOCK == 2
    targetPositionX = random(0, 3); // LEFT == 0, RIGHT == 1, NEUTRAL == 2
    targetPositionY = random(0, 3); // UP == 0, DOWN == 1, NEUTRAL == 2
    int audio_track;
    targetButton == 2? audio_track = targetPositionY + (3 * targetButton) + 1 : audio_track = targetPositionX + (3 * targetButton) + 1;
    String filename = getCommandScreen();
    player.playFolder(1, audio_track);
    reader.drawBMP(filename.c_str(), tft, 0, 0);
    startTime = millis();
    
    while (millis() - startTime < timeLimit) {
        int input = getPressedButton();
        int xInput = getXPosition();
        int yInput = getYPosition();
        
        if (input >= 0) {
            if (validatePress(input) && validatePosition(xInput, yInput)) {
                
                String name = getSuccessScreen();
                player.playFolder(2, audio_track);
                reader.drawBMP(name.c_str(), tft, 0, 0);
                levelUp();
                delay(200);
                return true;  // Move to next level
            } else {
                return false;  // Wrong Move
            }
        }
    } return false; // Timeout
}

bool validatePress(int input) {
    if (input == targetButton) {
        return true;
    } else {
        return false;
    }
}

bool validatePosition(int xInput, int yInput) {
    if(targetButton == 2) {// if a block
      if(yInput == targetPositionY) return true;
      return false;
    }
    else if(targetButton == 0 || targetButton == 1) {// if a punch or kick
      if(xInput == targetPositionX) return true;
      return false;
    }
}

String getCommandScreen() {
    if(targetButton == 2) {// if a block
      return String(targetPositionY + (3 * targetButton) + (9 * level)) + ".bmp";
    }
    else if(targetButton == 0 || targetButton == 1) {// if a punch or kick
      return String(targetPositionX + (3 * targetButton) + (9 * level)) + ".bmp";
    }
}

String getSuccessScreen() {
  if(targetButton == 2) {// if a block
    return String(3000 + targetPositionY + (3 * targetButton) + (13 * level)) + ".bmp";
  }
    else if(targetButton == 0 || targetButton == 1) {// if a punch or kick
      return String(3000 + targetPositionX + (3 * targetButton) + (13 * level)) + ".bmp";
  }
}