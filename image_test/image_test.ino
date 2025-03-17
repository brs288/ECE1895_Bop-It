#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_CS     10   // Chip select for OLED
#define SD_CS       4    // Chip select for SD Card
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS, -1, -1);

File bmpFile;

void setup() {
    Serial.begin(115200);
    pinMode(SD_CS, OUTPUT);
    pinMode(OLED_CS, OUTPUT);

    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card initialization failed!");
        return;
    }
    Serial.println("SD Card initialized.");

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 initialization failed!");
        return;
    }
    display.clearDisplay();
    display.display();
    
    showBMP("test.bmp");  // Load BMP file from SD
}

void loop() {
}

void showBMP(const char *filename) {
    bmpFile = SD.open(filename);
    if (!bmpFile) {
        Serial.println("BMP file not found.");
        return;
    }

    // Read BMP Header
    uint8_t header[54];
    bmpFile.read(header, 54);
    
    // Get width and height from BMP header
    int bmpWidth = *(int*)&header[18];
    int bmpHeight = *(int*)&header[22];

    Serial.print("BMP Width: "); Serial.println(bmpWidth);
    Serial.print("BMP Height: "); Serial.println(bmpHeight);

    // Read BMP data (assuming 1-bit monochrome BMP)
    for (int y = bmpHeight - 1; y >= 0; y--) {
        for (int x = 0; x < bmpWidth / 8; x++) {
            uint8_t pixelByte = bmpFile.read();
            for (int bit = 0; bit < 8; bit++) {
                int pixel = (pixelByte & (1 << (7 - bit))) ? 1 : 0;
                display.drawPixel(x * 8 + bit, y, pixel);
            }
        }
    }

    bmpFile.close();
    display.display();
}
