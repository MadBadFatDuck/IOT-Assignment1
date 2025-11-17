#include "game.h"
#include <Arduino.h>

// Dichiarazioni Estere
extern void lcd_clear();
extern void lcd_print_lines(const char* line1, const char* line2);
extern void lcd_print_sequence(const int sequence[], int length);
extern void pulse_led_ls();
extern void set_led_ls(bool state);
extern void enterDeepSleep(); 

// Variabili di Stato Globali 
int array[MAXARRAY]; 
GameState currentState = GAME_INIT; 
unsigned long stateStartTime = 0; 
int inputIndex = 0; 
int userInput[MAXARRAY]; 
int lastButtonState[4] = {LOW, LOW, LOW, LOW}; 
unsigned long lastDebounceTime = 0; 
const long debounceDelay = 200; 
const long WIN_DISPLAY_TIME = 3000;
const long SLEEP_TIMEOUT = 10000; 
const long T1_MINIMUM = 10;         
const int POT_MAX_READING = 675;
bool display_updated = false; 
bool waitingForButtonRelease = false;
int currentScore = 0;
int difficultyLevel = 1; 
int last_difficulty_level = 0; 
long T1_initial = 5000;  
long T1_current = T1_initial;
float Factor_F_base = 0.85; 
float Factor_F;
bool ls_on_during_game_over = false; 

// Funzioni Interne

void genArray(){
    int x, y = 0, z = 0, w;
    x = random(1, 5); do { y = random(1, 5); } while (y == x);
    do { z = random(1, 5); } while (z == x || z == y);
    w = 1; while (w == x || w == y || w == z) { w++; } 
    array[0] = x; array[1] = y; array[2] = z; array[3] = w;
    for(int i = 0; i < MAXARRAY; i++) Serial.print(array[i]);
    Serial.println();
}

void calculateFactorF() {
    float difficulty_reduction = (difficultyLevel - 1) * 0.1; 
    Factor_F = Factor_F_base * (1.0 - difficulty_reduction);
}

void initializeGame() {
    randomSeed(micros()); 
    currentScore = 0;
    T1_current = T1_initial;
    currentState = GAME_INIT; 
    stateStartTime = millis(); 
    set_led_ls(false);
    ls_on_during_game_over = false;
    display_updated = false; 
    last_difficulty_level = 0;
}


// Stati di gioco

void handleGameInit(int pin1_state, int sensor_value) {
    
    difficultyLevel = map(sensor_value, 0, POT_MAX_READING, 1, 4);
    if (difficultyLevel > 4) difficultyLevel = 4; 

    if (!display_updated || difficultyLevel != last_difficulty_level) {
        if (!display_updated) lcd_clear();
        
        char line2[17];
        
        lcd.setCursor(0, 0); 
        lcd.print("Welcome to TOS!");
        
        lcd.setCursor(0, 1);
        sprintf(line2, "L=%d Press B1 Start", difficultyLevel);
        lcd.print(line2);
        
        display_updated = true; 
        last_difficulty_level = difficultyLevel;
    }
    
    pulse_led_ls(); 
    if (millis() - stateStartTime > SLEEP_TIMEOUT) {
        lcd_print_lines("Sleeping...", "Press B1 to wake");
        set_led_ls(false);
        enterDeepSleep();

        initializeGame();
        waitingForButtonRelease = true;
        return;
        
    }

    if (waitingForButtonRelease) {
        if (pin1_state == LOW) {
            waitingForButtonRelease = false;
        }
        return; 
    }
    
    if (pin1_state == HIGH) {
        calculateFactorF(); 
        
        lcd_print_lines("Go!", "Memorizza ora");
        delay(1000); 
        genArray(); 
        currentState = GAME_SHOW_SEQUENCE;
        stateStartTime = millis();
        set_led_ls(false); 
        
        display_updated = false; 
    }
    
}

void handleShowSequence() {
    unsigned long currentTime = millis();
    
    if (currentTime - stateStartTime < T1_current) { 
        if (!display_updated) {
            lcd_print_sequence(array, MAXARRAY); 
            display_updated = true;
        }
        
    } else {
        lcd_clear(); 
        lcd_print_lines("Input:", "Go!");
        
        inputIndex = 0;
        stateStartTime = currentTime;
        currentState = GAME_WAIT_INPUT;
        display_updated = false; 
    }
}


void handleWaitInput(int p1, int p2, int p3, int p4) {
    unsigned long currentTime = millis();
    
    if (inputIndex >= MAXARRAY) {
        stateStartTime = currentTime;
        currentState = GAME_CHECK_RESULT;
        return;
    }
    
    int currentPinStates[4] = {p1, p2, p3, p4}; 
    
    for (int i = 0; i < 4; i++) {
        if (currentPinStates[i] == HIGH && lastButtonState[i] == LOW) {
            if (currentTime - lastDebounceTime > debounceDelay) {
            
                int input_number = i + 1;
                userInput[inputIndex] = input_number; 
                
                lcd.setCursor(7 + inputIndex * 2, 0); 
                lcd.print(input_number);
                
                inputIndex++;
                lastDebounceTime = currentTime; 
                break; 
            }
        }
        lastButtonState[i] = currentPinStates[i]; 
    }
}

void handleCheckResult() {
    bool correct = true;
    
    if (inputIndex < MAXARRAY) {
        correct = false; 
    } else {
    
        for (int i = 0; i < MAXARRAY; i++) {
            if (userInput[i] != array[i]) {
                correct = false;
                break;
            }
        }
    }
    
    if (correct) {
        if (!display_updated) {
            currentScore++; 
            
            T1_current = (long)(T1_current * Factor_F); 
            if (T1_current < T1_MINIMUM) T1_current = T1_MINIMUM; 
            
            char line1[17]; char line2[17];
            sprintf(line1, "GOOD! Score: %d", currentScore);
            sprintf(line2, "T1 Next: %ldms", T1_current);
            lcd_print_lines(line1, line2);
            
            display_updated = true;
            stateStartTime = millis(); 
        }
        
        if (millis() - stateStartTime > WIN_DISPLAY_TIME) { 
             genArray(); 
             stateStartTime = millis();
             currentState = GAME_SHOW_SEQUENCE;
             display_updated = false; 
        }
    } else {

        if (!ls_on_during_game_over) {
            set_led_ls(true); 
            stateStartTime = millis(); 
            ls_on_during_game_over = true;
        }
        
        if (millis() - stateStartTime > 2000) { 
            set_led_ls(false); 
            stateStartTime = millis(); 
            currentState = GAME_OVER;
        }
    }
}

void handleGameOver() {
    if (!display_updated) {
        char line1[17]; 
        char line2[17];
        
        sprintf(line1, "Game Over!");
        sprintf(line2, "Final Score: %d", currentScore);
        lcd_print_lines(line1, line2);
        
        display_updated = true;
    }
    
    if (millis() - stateStartTime > 10000) {
        initializeGame(); 
    }
}

void updateGame(int pin1_state, int pin2_state, int pin3_state, int pin4_state, int sensor_value) {
    
    if (currentState == GAME_INIT) {
        pulse_led_ls();
    } else {
        if (currentState != GAME_CHECK_RESULT) {
             set_led_ls(false);
        }
    }
    
    switch (currentState) {
        case GAME_INIT:
            handleGameInit(pin1_state, sensor_value);
            break;
            
        case GAME_SHOW_SEQUENCE:
            handleShowSequence();
            break;
            
        case GAME_WAIT_INPUT:
            handleWaitInput(pin1_state, pin2_state, pin3_state, pin4_state);
            break;
            
        case GAME_CHECK_RESULT:
            handleCheckResult();
            break;
            
        case GAME_OVER:
            handleGameOver();
            break;
    }
}