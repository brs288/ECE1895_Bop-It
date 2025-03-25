

/*

  Test version of the game that uses the 3.2 inch rgb tft display
  Updates include changing the communication protocol to SPI
  It may be necessary to make further adjustments to font sizes, colors, timing, position of the elements on screen etc
  A separate test will be done to see the plausibility of writing from the sd card reader.

*/

#include <DFRobotDFPlayerMini.h>
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_ImageReader.h> // Image-reading functions


#define SD_CS   4 // SD card select pin
#define TFT_CS     10  // Chip Select
#define TFT_DC     9  // Data/Command

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys

DFRobotDFPlayerMini myDFPlayer;

// Button Pins
const int button1 = 4;
const int button2 = 3;
const int button3 = 2;

int level = 1;
int timeLimit = 3000;  // Initial time limit in milliseconds
int timeInitial = timeLimit;
unsigned long startTime;
int targetButton;
bool gameRunning = false;

void setup() {
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    pinMode(button3, INPUT);
    
    // Initialize the TFT display
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);  // Clear screen
    tft.setRotation(3);  // Adjust rotation for landscape mode

    if(!SD.begin(SD_CS, SD_SCK_MHZ(25))) { // ESP32 requires 25 MHz limit
      for(;;); // Fatal error, do not continue
    }

    // Audio
    Serial.begin(9600);
    if (!myDFPlayer.begin(Serial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
      while(true);
    }
    resetGame();  // Start in "Press Start" state
}

void loop() {
    myDFPlayer.play(1);  //Play the first mp3
    if (!gameRunning) {
        waitForStart();
        countdown();
        gameRunning = true;
        level = 1;
        timeLimit = 3000;
    }
    
    playLevel();
}

void waitForStart() {
    showMessage("Press Start", ILI9341_YELLOW);
    while (true) {
        if (digitalRead(button1) == HIGH || digitalRead(button2) == HIGH || digitalRead(button3) == HIGH) {
            delay(200);  // Debounce delay
            return;
        }
    }
}

void countdown() {
}

void playLevel() {
    targetButton = random(1, 4);
    startTime = millis();
    
    while (millis() - startTime < timeLimit) {
        showGameStatus();

        int pressedButton = getPressedButton();
        if (pressedButton > 0) {
            if (validatePress(pressedButton)) {
                return;  // Move to next level
            } else {
                return;  // Game over
            }
        }
    }

    gameOver();
}

void showGameStatus() {
     reader.drawBMP("/results/level_0/down_kick.bmp", tft, 0, 0);
}

int getPressedButton() {
    if (digitalRead(button1) == HIGH) return 1;
    if (digitalRead(button2) == HIGH) return 2;
    if (digitalRead(button3) == HIGH) return 3;
    return 0;
}

bool validatePress(int buttonPressed) {
    if (buttonPressed == targetButton) {
        showMessage("Correct!", ILI9341_GREEN);
        delay(500);
        levelUp();
        return true;
    } else {
        showMessage("Wrong!", ILI9341_RED);
        delay(1000);
        gameOver();
        return false;
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

void gameOver() {
    reader.drawBMP("/results/level_0/down_block.bmp", tft, 0, 0);
}


void victoryScreen() {
    reader.drawBMP("/results/level_0/down_kick.bmp", tft, 0, 0);
}

void resetGame() {
    gameRunning = false;
}

void showMessage(String msg, uint16_t color) {
 
}
