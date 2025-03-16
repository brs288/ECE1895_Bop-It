/*

  Test version of the game that uses the 3.2 inch rgb tft display
  Updates include changing the communication protocol to SPI
  It may be necessary to make further adjustments to font sizes, colors, timing, position of the elements on screen etc
  A separate test will be done to see the plausibility of writing from the sd card reader.

*/


#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// TFT SPI Pins
#define TFT_CS     10  // Chip Select
#define TFT_RST    9   // Reset
#define TFT_DC     8   // Data/Command

Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

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
    display.begin();
    display.fillScreen(ILI9341_BLACK);  // Clear screen
    display.setRotation(3);  // Adjust rotation for landscape mode

    resetGame();  // Start in "Press Start" state
}

void loop() {
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
    for (int i = 3; i > 0; i--) {
        display.fillScreen(ILI9341_BLACK);
        
        display.setTextSize(2);
        display.setTextColor(ILI9341_WHITE);
        display.setCursor(100, 80);
        display.println("Starting in");

        display.setTextSize(5);
        display.setCursor(140, 120);
        display.println(String(i));

        display.display();
        delay(1000);
    }
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
    display.fillScreen(ILI9341_BLACK);
    
    display.setTextSize(2);
    display.setTextColor(ILI9341_WHITE);

    display.setCursor(10, 10);
    display.println("Level: " + String(level));

    display.setCursor(10, 50);
    display.println("Speed: " + String(timeLimit / 1000.0) + "s");

    display.setTextSize(3);
    display.setCursor(80, 100);
    display.setTextColor(ILI9341_CYAN);
    display.println("Press " + String(targetButton));

    // Progress Bar
    int barWidth = map(millis() - startTime, 0, timeLimit, 320, 0);
    display.fillRect(10, 200, barWidth, 10, ILI9341_GREEN); 

    display.display();
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
    display.fillScreen(ILI9341_BLACK);
    
    display.setTextSize(3);
    display.setTextColor(ILI9341_RED);

    display.setCursor(70, 80);
    display.println("Game Over!");

    display.setTextSize(2);
    display.setTextColor(ILI9341_WHITE);
    display.setCursor(90, 140);
    display.println("Score: " + String(level));

    display.display();
    delay(3000);
    resetGame();
}

void victoryScreen() {
    display.fillScreen(ILI9341_BLACK);
    
    display.setTextSize(3);
    display.setTextColor(ILI9341_YELLOW);

    display.setCursor(90, 80);
    display.println("Victory!");

    display.setTextSize(2);
    display.setTextColor(ILI9341_WHITE);
    display.setCursor(90, 140);
    display.println("Score: 99");

    display.display();
    delay(3000);
}

void resetGame() {
    gameRunning = false;
}

void showMessage(String msg, uint16_t color) {
    display.fillScreen(ILI9341_BLACK);
    
    display.setTextSize(3);
    display.setTextColor(color);
    display.setCursor(70, 100);
    display.println(msg);
    
    display.display();
}
