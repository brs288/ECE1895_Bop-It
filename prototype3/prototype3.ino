#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_ImageReader.h> // Image-reading functions
#include <DFRobotDFPlayerMini.h>


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
int timeLimit = 5000;
int timeInitial = timeLimit;
unsigned long startTime;
unsigned int targetButton;
unsigned int targetPositionX;
unsigned int targetPositionY;
bool gameRunning = false;

void setup(void) {

  // Initialize Pin Mode
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
  player.volume(10);

  resetGame();  // Start in "Press Start" state
  delay(2000); // Pause 2 seconds before moving on to loop()
}

void loop() {
    if (!gameRunning) {
        waitForStart();
        gameRunning = true;
        level = 1;
        timeLimit = 3000;
    }
    
    playLevel();
}

void waitForStart() {
    while (true) {
        if (digitalRead(START) == HIGH) {
            delay(200);  // Debounce delay
            return;
        }
    }
}

void levelUp() {
    if (level == 99) {
        victoryScreen();
        resetGame();
    } else {
        level++;
        if (level % 10 == 0) timeLimit = timeInitial * exp(-(1.75 / 100) * level + 1.1) / 3.0;
        delay(500);
    }
}

void displayImage() {
    unsigned int index = 0;

    reader.drawBMP("/results/level_0/down_block.bmp", tft, 0, 0);
}

void resetGame() {
    gameRunning = false;
}

void gameOver() {
    reader.drawBMP("/results/level_0/down_block.bmp", tft, 0, 0);
}


void victoryScreen() {
    reader.drawBMP("/results/level_0/down_kick.bmp", tft, 0, 0);
}

int getPressedButton() {
    if (digitalRead(BLOCK == HIGH)) return 1;
    if (digitalRead(PUNCH == HIGH)) return 2;
    if (digitalRead(KICK == HIGH)) return 3;
    return 0;
}

int getXPosition() {
    if (analogRead(JSX < 256)) return 1; // LEFT
    if (analogRead( JSX > 768)) return 2; // RIGHT
    return 0; // NEUTRAL
}

int getYPosition() {
    if (analogRead(JSY < 256)) return 1; // DOWN
    if (analogRead(JSY > 768)) return 2; // UP
    return 0; // NEUTRAL
}

void playLevel() {
    targetButton = random(1, 4); // BLOCK == 1, PUNCH == 2, KICK == 3 , 0 -> no input
    targetPositionX = random(0, 3); // LEFT == 1, RIGHT == 2, NEUTRAL == 0
    targetPositionY = random(0, 3); // DOWN == 1, UP == 2, NEUTRAL == 0

    startTime = millis();
    
    while (millis() - startTime < timeLimit) {
        int input = getPressedButton();
        int xInput = getXPosition();
        int yInput = getYPosition();
        if (input > 0) {
            if (validatePress(input) && validatePosition(xInput, yInput)) {
                
                levelUp();
                delay(200);
                return;  // Move to next level
            } else {
                delay(200);
                gameOver();
                return;  // Game over
            }
        }
    }
}

bool validatePress(int input) {
    if (input == targetButton) {
        return true;
    } else {
        return false;
    }
}

bool validatePosition(int xInput, int yInput) {
    if(targetButton == 0) {// if a block
      if(yInput == targetPositionY) return true;
      return false;
    }
    else if(targetButton == 1 || targetButton == 2) {// if a punch or kick
      if(xInput == targetPositionX) return true;
      return false;
    }
}