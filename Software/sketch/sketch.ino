#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MIDPOINT 1900
#define SAMPLES 256

const int button1 = 18; // GP18
const int button2 = 28; // GP28

const int micPin = A0; // ADC0 on GPIO26

long sum;
int loudness;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(10); // wait for Serial Monitor
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  analogReadResolution(12); // 12-bit ADC (0–4095)
}

void loop() {
  sum = 0;

  display.setCursor(0, 0);

  for (int i = 0; i < SAMPLES; i++) 
  {
    sum += abs(analogRead(micPin) - MIDPOINT);
  }

  loudness = sum / SAMPLES;
  if (loudness < 0) loudness = 0;
  display.clearDisplay();
  display.print(loudness);

  display.display();
  delay(300);
}