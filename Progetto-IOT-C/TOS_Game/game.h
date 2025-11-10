#ifndef GAME_H
#define GAME_H

#include "hardware.h" 

#define MAXARRAY 4 

extern int array[MAXARRAY];

typedef enum {
    GAME_INIT,          
    SET_DIFFICULTY,
    GAME_SHOW_SEQUENCE, 
    GAME_WAIT_INPUT,    
    GAME_CHECK_RESULT,
    ROUND_WIN,  
    GAME_OVER           
} GameState;

void initializeGame();
void updateGame(int pin1_state, int pin2_state, int pin3_state, int pin4_state, int sensor_value); 


void genArray(); 
void handleShowSequence(); 
void handleWaitInput(int pin1_state, int pin2_state, int pin3_state, int pin4_state);
void handleCheckResult(); 
void handleGameOver();

#endif 