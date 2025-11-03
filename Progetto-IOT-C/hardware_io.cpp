#include <Arduino.h>
#include "hardware_io.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>


const int LCD_ADDR = 0x27;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

static int brightness = 0;
static int fadeAmount = 5;
static unsigned long lastPulseTime = 0;
const int PULSE_INTERVAL_MS = 20;

//API

void io_setup() {
    pinMode(PIN_B1, INPUT_PULLUP);
    pinMode(PIN_LS, OUTPUT);
    pinMode(PIN_POT, INPUT);

    Wire.begin();

    lcd.init();
    lcd.backlight();
    lcd.print("TOS Loading...");

    Serial.begin(9600);
}

int read_button(int pin) {
    return !digitalRead(pin);
}

void lcd_display(const char* line1, const char* line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);

    Serial.print("LCD: ");
    Serial.print(line1);
    Serial.print(" | ");
    Serial.println(line2);
}

int read_potentiometer() {
    return analogRead(PIN_POT);
}

void pulse_led() {
    unsigned long currentTime = millis();

    if (currentTime - lastPulseTime >= PULSE_INTERVAL_MS) {
        analogWrite(PIN_LS, brightness);

        brightness = brightness + fadeAmount;

        if (brightness <= 0 || brightness >= 255) {
            fadeAmount = -fadeAmount;
        }

        lastPulseTime = currentTime;
    }
}