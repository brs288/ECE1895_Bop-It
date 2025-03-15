#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int button1 = 4;  // Swapped to pin 4
const int button2 = 3;  // Swapped to pin 3
const int button3 = 2;  // Swapped to pin 2

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

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (true);  // Halt if display fails to initialize
    }

    display.clearDisplay();
    display.display();
    
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
    showMessage("Press\nStart", true);  // Two-line format
    while (true) {
        if (digitalRead(button1) == HIGH || digitalRead(button2) == HIGH || digitalRead(button3) == HIGH) {
            delay(200);  // Debounce delay
            return;
        }
    }
}

void countdown() {
    for (int i = 3; i > 0; i--) {
        display.clearDisplay();
        
        display.setTextSize(2);  // Reduced size to fit properly
        display.setTextColor(WHITE);
        display.setCursor(25, 20);  // Centered properly
        display.println("Start in");
        
        display.setTextSize(3);  // Larger font for the countdown number
        display.setCursor(55, 40);
        display.println(String(i));

        display.display();
        delay(1000);
    }
}

void playLevel() {
    targetButton = random(1, 4);  // Generate a random button (1, 2, or 3)
    startTime = millis();
    
    while (millis() - startTime < timeLimit) {
        showGameStatus();  // Continuously update display

        int pressedButton = getPressedButton();
        if (pressedButton > 0) {
            if (validatePress(pressedButton)) {
                return;  // Move to next level
            } else {
                return;  // Game over
            }
        }
    }

    gameOver();  // If time runs out, game over
}

void showGameStatus() {
    display.clearDisplay();
    
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.println("Level: " + String(level));

    display.setCursor(0, 20);  // Moved Speed down slightly
    display.println("Speed: " + String(timeLimit/1000.) + "s");

    display.setTextSize(2);
    display.setCursor(0, 40);  // Lowered "Press Button" for better positioning
    display.println("Press " + String(targetButton));

    // Draw a visual timer bar at the bottom
    int barWidth = map(millis() - startTime, 0, timeLimit, SCREEN_WIDTH, 0);
    display.fillRect(0, SCREEN_HEIGHT - 5, barWidth, 3, WHITE); 

    display.display();
}

int getPressedButton() {
    if (digitalRead(button1) == HIGH) return 1;
    if (digitalRead(button2) == HIGH) return 2;
    if (digitalRead(button3) == HIGH) return 3;
    return 0;  // No button pressed
}

bool validatePress(int buttonPressed) {
    if (buttonPressed == targetButton) {
        showMessage("Correct!", true);
        delay(500);
        levelUp();
        return true;
    } else {
        showMessage("Wrong!", true);
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
        if (level % 10 == 0) timeLimit = timeInitial*exp(-(1.75/100)*level + 1.1)/3.;
        delay(500);
    }
}

void gameOver() {
    display.clearDisplay();
    
    display.setTextSize(2);
    display.setTextColor(WHITE);

    display.setCursor(15, 10);
    display.println("Game Over!");

    display.setTextSize(1);
    display.setCursor(25, 40);
    display.println("Score: " + String(level));

    display.display();
    delay(3000);
    resetGame();
}

void victoryScreen() {
    display.clearDisplay();
    
    display.setTextSize(2);
    display.setTextColor(WHITE);

    display.setCursor(20, 10);
    display.println("Victory!");

    display.setTextSize(1);
    display.setCursor(25, 40);
    display.println("Score: 99");

    display.display();
    delay(3000);
}

void resetGame() {
    gameRunning = false;
}

void showMessage(String msg, bool clear) {
    if (clear) display.clearDisplay();
    
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 25);
    display.println(msg);
    
    display.display();
}
