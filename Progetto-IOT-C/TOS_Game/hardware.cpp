// hardware.cpp
#include <Arduino.h>
#include "hardware.h" 
#include <Wire.h> 

// DEFIZIONE e INIZIALIZZAZIONE LCD (Usa il tuo indirizzo I2C, es. 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// --- Implementazioni LCD (già concordate) ---

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


// --- Implementazioni LED (MANCANTI: RISOLVE GLI ERRORI) ---

// Accende/spegne il LED rosso LS
void set_led_ls(bool state) {
    digitalWrite(PIN_LS, state ? HIGH : LOW);
}

// Accende/spegne un LED verde (index 1..4)
void set_led_l(int index, bool state) {
    int pin = 0;
    if (index == 1) pin = PIN_L1;
    else if (index == 2) pin = PIN_L2;
    else if (index == 3) pin = PIN_L3;
    else if (index == 4) pin = PIN_L4;
    
    if (pin != 0) {
        digitalWrite(pin, state ? HIGH : LOW);
    }
}

// Fa pulsare il LED rosso LS (richiede PIN_LS su un pin PWM)
void pulse_led_ls() {
    static unsigned long last_time = 0;
    static int brightness = 0;
    static int fadeAmount = 5;
    
    // Aggiorna la luminosità (effetto sfumato)
    if (millis() - last_time > 30) { 
        analogWrite(PIN_LS, brightness);
        brightness = brightness + fadeAmount;
        
        if (brightness <= 0 || brightness >= 255) {
            fadeAmount = -fadeAmount; 
        }
        last_time = millis();
    }
}


// --- Funzione Principale di Setup (AGGIORNA I PIN DEI LED) ---

void setupHardware() {
  // Pin dei pulsanti (INPUT)
  pinMode(PIN1, INPUT);
  pinMode(PIN2, INPUT);
  pinMode(PIN3, INPUT);
  pinMode(PIN4, INPUT);
  
  // Pin dei LED (OUTPUT)
  pinMode(PIN_LS, OUTPUT); // LED Rosso
  pinMode(PIN_L1, OUTPUT);
  pinMode(PIN_L2, OUTPUT);
  pinMode(PIN_L3, OUTPUT);
  pinMode(PIN_L4, OUTPUT);

  // Setup dell'LCD
  lcd_setup();
    
  // Inizializzazione della seriale (mantenuta per debug)
  Serial.begin(9600);
}

// Implementazione di getButtonState e readPotentiometer
int getButtonState(int pin) {
  return digitalRead(pin);
}

int readPotentiometer() {
  return analogRead(SENSOR);
}