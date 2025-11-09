#ifndef HARDWARE_H
#define HARDWARE_H

#include <LiquidCrystal_I2C.h> 

// Pin dei Pulsanti e Sensore
#define PIN1 2
#define PIN2 3
#define PIN3 4
#define PIN4 5
#define SENSOR A0

// Pin dei LED (LS deve essere PWM, ~)
#define PIN_LS 10 
#define PIN_L1 6
#define PIN_L2 7
#define PIN_L3 8
#define PIN_L4 9

// Dichiarazione Esterna LCD
extern LiquidCrystal_I2C lcd; 

// Prototipi Hardware (LCD/LED/Input)
void setupHardware(); 
int getButtonState(int pin);
int readPotentiometer();

// Prototipi per la visualizzazione LCD
void lcd_setup();
void lcd_clear();
void lcd_print_lines(const char* line1, const char* line2);
void lcd_print_sequence(const int sequence[], int length);

// Prototipi per la gestione dei LED (MANCANTI)
void pulse_led_ls(); 
void set_led_ls(bool state); 
void set_led_l(int index, bool state); // index 1-4

#endif // HARDWARE_H