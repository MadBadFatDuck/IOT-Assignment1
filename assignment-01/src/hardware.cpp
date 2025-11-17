// hardware.cpp
#include <Arduino.h>
#include "hardware.h" 
#include <Wire.h> 

// Librerie necessarie per il Deep Sleep
#include <avr/sleep.h> 
#include <avr/wdt.h>   

// DEFIZIONE e INIZIALIZZAZIONE LCD (Usa il tuo indirizzo I2C, es. 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// LCD

void lcd_setup() {
    Wire.begin();
    lcd.init();
    lcd.backlight();
}

void lcd_clear() {
    lcd.clear();
}

void lcd_print_lines(const char* line1, const char* line2) {
    lcd_clear();
    lcd.setCursor(0, 0); 
    lcd.print(line1);
    lcd.setCursor(0, 1); 
    lcd.print(line2);
}

void lcd_print_sequence(const int sequence[], int length) {
    char sequence_str[length + 1];
    for (int i = 0; i < length; i++) {
        sequence_str[i] = sequence[i] + '0';
    }
    sequence_str[length] = '\0';
    
    lcd_clear();
    lcd.setCursor(3, 0); 
    lcd.print("MEMORIZZA:");
    lcd.setCursor(6, 1); 
    lcd.print(sequence_str);
}


// LED

// Accende/spegne il LED rosso LS
void set_led_ls(bool state) {
    digitalWrite(PIN_LS, state ? HIGH : LOW);
}


// Pulse del led Rosso
void pulse_led_ls() {
    static unsigned long last_time = 0;
    static int brightness = 0;
    static int fadeAmount = 5;
    
    if (millis() - last_time > 30) { 
        analogWrite(PIN_LS, brightness);
        brightness = brightness + fadeAmount;
        
        if (brightness <= 0 || brightness >= 255) {
            fadeAmount = -fadeAmount; 
        }
        last_time = millis();
    }
}


// Deep Sleep

void wakeUpISR() {
  sleep_disable();
  
}

// DeepSleep
void enterDeepSleep() {

  attachInterrupt(digitalPinToInterrupt(PIN1), wakeUpISR, RISING); 
  
  ADCSRA &= ~(1<<ADEN); 
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  
  sleep_cpu();
  sleep_disable(); 

  ADCSRA |= (1<<ADEN);
}


// SETUP

void setupHardware() {
  // Pin dei pulsanti in input
  pinMode(PIN1, INPUT);
  pinMode(PIN2, INPUT);
  pinMode(PIN3, INPUT);
  pinMode(PIN4, INPUT);
  
  // Pin led rosso di output
  pinMode(PIN_LS, OUTPUT); 
  
  // Setup dell'LCD
  lcd_setup();
    
  // Inizializzazione della seriale
  Serial.begin(9600);
}

// Implementazione di getButtonState e readPotentiometer
int getButtonState(int pin) {
  return digitalRead(pin);
}

int readPotentiometer() {
  return analogRead(SENSOR);
}