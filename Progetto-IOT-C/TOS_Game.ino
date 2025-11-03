#include "hardware_io.h"
#include "game_fsm.h"

void setup() {
    io_setup();
    fsm_setup();
}

void loop() {
    process_state();
}