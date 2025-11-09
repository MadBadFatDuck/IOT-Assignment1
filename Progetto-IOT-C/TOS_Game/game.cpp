// game.cpp
#include "game.h"
#include <Arduino.h>

// --- Dichiarazioni Estere (Invariate) ---
extern void lcd_clear();
extern void lcd_print_lines(const char* line1, const char* line2);
extern void lcd_print_sequence(const int sequence[], int length);
extern void pulse_led_ls();
extern void set_led_ls(bool state);
extern void set_led_l(int index, bool state); 
// ---

// --- Variabili di Stato Globali (Invariate) ---
int array[MAXARRAY]; 
GameState currentState = GAME_INIT; 
unsigned long stateStartTime = 0; 
int inputIndex = 0; 
int userInput[MAXARRAY]; 
int lastButtonState[4] = {LOW, LOW, LOW, LOW}; 
unsigned long lastDebounceTime = 0; 
const long debounceDelay = 200; 
const long WIN_DISPLAY_TIME = 3000;

bool display_updated = false; 
int currentScore = 0;
int difficultyLevel = 1; 
int last_difficulty_level = 0; 
long T1_initial = 5000;  
long T1_current = T1_initial;
float Factor_F_base = 0.85; 
float Factor_F;
bool ls_on_during_game_over = false; 

// --- Funzioni Interne (genArray, calculateFactorF, initializeGame - Invariate) ---

void genArray(){
    int x, y = 0, z = 0, w;
    x = random(1, 5); do { y = random(1, 5); } while (y == x);
    do { z = random(1, 5); } while (z == x || z == y);
    w = 1; while (w == x || w == y || w == z) { w++; } 
    array[0] = x; array[1] = y; array[2] = z; array[3] = w;
    Serial.print("Nuova Sequenza (Debug): ");
    for(int i = 0; i < MAXARRAY; i++) Serial.print(array[i]);
    Serial.println();
}

void calculateFactorF() {
    float difficulty_reduction = (difficultyLevel - 1) * 0.1; 
    Factor_F = Factor_F_base * (1.0 - difficulty_reduction);
    Serial.print("Fattore F applicato: "); Serial.println(Factor_F);
}

void initializeGame() {
    randomSeed(micros()); 
    currentScore = 0;
    T1_current = T1_initial;
    currentState = GAME_INIT; 
    stateStartTime = millis(); 
    set_led_ls(false);
    ls_on_during_game_over = false;
    for(int i = 1; i <= MAXARRAY; i++) set_led_l(i, false);
    display_updated = false; 
    last_difficulty_level = 0;
}


// --- Gestione Stati di Gioco ---

void handleGameInit(int pin1_state) {
    if (!display_updated) {
        lcd_print_lines("Welcome to TOS!", "Press B1 to Start");
        display_updated = true;
    }
    pulse_led_ls(); 
    
    if (pin1_state == HIGH) {
        currentState = SET_DIFFICULTY;
        stateStartTime = millis();
        set_led_ls(false); 
        display_updated = false; 
    }
}

void handleSetDifficulty(int sensor_value, int pin1_state) {
    difficultyLevel = map(sensor_value, 0, 1023, 1, 4);
    
    if (!display_updated || difficultyLevel != last_difficulty_level) {
        char line1[17]; char line2[17];
        sprintf(line1, "Set Diff. Level:");
        sprintf(line2, "L=%d (Press B1)", difficultyLevel);
        lcd_print_lines(line1, line2); 
        
        display_updated = true;
        last_difficulty_level = difficultyLevel;
    }
    
    if (pin1_state == HIGH && millis() - lastDebounceTime > debounceDelay) {
        lastDebounceTime = millis();
        calculateFactorF(); 
        
        lcd_print_lines("Go!", "Memorizza ora");
        delay(1000); 
        genArray(); 
        currentState = GAME_SHOW_SEQUENCE;
        stateStartTime = millis();
        display_updated = false; 
    }
}

void handleShowSequence() {
    unsigned long currentTime = millis();
    
    if (currentTime - stateStartTime < 3000) { 
        if (!display_updated) {
            lcd_print_sequence(array, MAXARRAY); 
            display_updated = true;
        }
        
    } else {
        lcd_clear(); 
        char line2[17];
        sprintf(line2, "T1: %ldms", T1_current);
        lcd_print_lines("Input: _ _ _ _", line2);
        
        inputIndex = 0;
        stateStartTime = currentTime;
        currentState = GAME_WAIT_INPUT;
        display_updated = false; 
    }
}

void checkAndLightLED(int input_number) {
    set_led_l(input_number, true);
}


void handleWaitInput(int p1, int p2, int p3, int p4) {
    unsigned long currentTime = millis();
    
    if (currentTime - stateStartTime > T1_current) {
        currentState = GAME_CHECK_RESULT; 
        return;
    }
    
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
                checkAndLightLED(input_number);
                
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
    
    // 1. Controllo Fallimento T1 o Mancanza Input
    if (inputIndex < MAXARRAY || millis() - stateStartTime > T1_current) {
        correct = false;
    } else {
        // 2. Controllo Sequenza
        for (int i = 0; i < MAXARRAY; i++) {
            if (userInput[i] != array[i]) {
                correct = false;
                break;
            }
        }
    }
    
    if (correct) {
        // SUCCESS: Avanti Prossimo Round
        
        if (!display_updated) {
            currentScore++; 
            
            T1_current = (long)(T1_current * Factor_F); 
            if (T1_current < 500) T1_current = 500; 
            
            char line1[17]; char line2[17];
            sprintf(line1, "GOOD! Score: %d", currentScore);
            sprintf(line2, "T1: %ldms", T1_current);
            lcd_print_lines(line1, line2);
            
            for(int i = 1; i <= MAXARRAY; i++) set_led_l(i, false);
            
            display_updated = true;
            // Reimposta il timer per l'attesa di visualizzazione (3 secondi)
            stateStartTime = millis(); 
        }
        
        // 3. Attesa e Transizione: USA IL TEMPO FISSO DI VISUALIZZAZIONE
        if (millis() - stateStartTime > WIN_DISPLAY_TIME) { 
             genArray(); 
             stateStartTime = millis();
             currentState = GAME_SHOW_SEQUENCE;
             display_updated = false; // Reset per GAME_SHOW_SEQUENCE
        }
    } else {
        // FAIL: Inizio Sequenza Game Over (Logica invariata)
        
        // Prima parte: Accendi LS per 2s (eseguita solo una volta)
        if (!ls_on_during_game_over) {
            set_led_ls(true); 
            stateStartTime = millis(); 
            ls_on_during_game_over = true;
        }
        
        // Seconda parte: LED LS acceso per 2 secondi
        if (millis() - stateStartTime > 2000) { 
            set_led_ls(false); 
            stateStartTime = millis(); 
            currentState = GAME_OVER;
        }
    }
}

void handleGameOver() {
    // Stampa solo una volta (la prima volta che entriamo in GAME_OVER)
    if (!display_updated) {
        char line1[17]; char line2[17];
        sprintf(line1, "Game Over!");
        sprintf(line2, "Final Score: %d", currentScore);
        lcd_print_lines(line1, line2);
        display_updated = true;
    }
    
    // Riavvia il gioco dopo 10 secondi
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
            handleGameInit(pin1_state);
            break;
            
        case SET_DIFFICULTY:
            handleSetDifficulty(sensor_value, pin1_state);
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