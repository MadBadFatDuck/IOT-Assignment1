#ifndef HARDWARE_IO_H
#define HARDWARE_IO_H

//PIN

// Pulsanti
const int PIN_B1 = 9;
const int PIN_B2 = 10;
const int PIN_B3 = 11;
const int PIN_B4 = 12;

// LED Rosso OUTPUT
const int PIN_LS = 2;

// Potenziometro
const int PIN_POT = A0;

//API

// Inizializzazione
void io_setup();

int read_button(int pin);

void lcd_display(const char* line1, const char* line2);

int read_potentiometer();

void pulse_led();

#endif