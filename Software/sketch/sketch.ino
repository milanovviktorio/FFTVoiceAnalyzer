#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Your measured idle value (DC offset) 
#define DC_OFFSET 1900
#define SAMPLE_RATE 8000
#define NUM_SAMPLES 256
#define MIC_PIN A0

#define LONG_PRESS_TIME 3000  // milliseconds

uint16_t samples[NUM_SAMPLES];

const int button1 = 18; // GP18
const int button2 = 28; // GP28

int state = 0;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(10); // wait for Serial Monitor
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  delay(200);
  display.clearDisplay(); 
  display.display();

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  analogReadResolution(12); // 12-bit ADC (0–4095)
}

void loop() {

  if (digitalRead(button1) == LOW) {
    Serial.println("START");
    unsigned long period_us = 1000000UL / SAMPLE_RATE;

    // Keep recording while button is held
    while (digitalRead(button1) == LOW) {

      // Collect one block of samples
      for (int i = 0; i < NUM_SAMPLES; i++) {
        unsigned long t0 = micros();
        samples[i] = analogRead(MIC_PIN);
        while (micros() - t0 < period_us);
      }

      // Send the block
      for (int i = 0; i < NUM_SAMPLES; i++) {
        int16_t centered = (int16_t)samples[i] - DC_OFFSET; 
        Serial.println(centered);
      }
    }
    Serial.println("END");
  }
  if (digitalRead(button2) == LOW) {

    // Debounce press
    delay(30);
    if (digitalRead(button2) != LOW) return;

    unsigned long pressStart = millis();

    // Stay here while button is held
    while (digitalRead(button2) == LOW) {

        // Long press reached
        if (millis() - pressStart >= LONG_PRESS_TIME) {
            // Debounce long press
            delay(50);
            if (digitalRead(button2) == LOW) {
                watchdog_reboot(0, 0, 0);
                while (1);
            }
        }

        delay(10); // stabilize sampling
    }

    // Debounce release
    delay(30);
    if (digitalRead(button2) == LOW) return;

    // Short press
    unsigned long pressDuration = millis() - pressStart;

    if (pressDuration < LONG_PRESS_TIME) 
    {
      if (Serial.available()) 
      { 
        String msg = Serial.readStringUntil('\n'); 
        msg.trim(); 
        if (msg.length() > 0) 
        { 
          float f0 = msg.toFloat();  
          display.setCursor(0,0); 
          display.print("F0: "); 
          display.println(f0);
          display.display();
        } 
      }
    }
  }
}

