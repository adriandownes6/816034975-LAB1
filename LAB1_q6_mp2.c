/*816034975 Adrian Downes*/
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define Interval 500
// FSM States
enum state { OFF, ON };

// Event Conditions
typedef enum {
    same_char_long,    // same character, interval > 500ms
    diff_char,             // different character
    same_char_short,   // same character, interval < 500ms
} event;

// Finite State Table Structure
typedef struct {
    enum state next_state;  // Next state after transition
    event (*event_fn)(char, char, uint32_t);  // Function to determine event
} fsm_transition_t;

// Variables for timing and character storage
volatile uint32_t counter = 0;
char last_char = 0;

// Initialize UART
void uart_init_setup() {
    gpio_set_function(0, GPIO_FUNC_UART); // TX pin
    gpio_set_function(1, GPIO_FUNC_UART); // RX pin
}

// Event Functions to determine the event type
event check_event(char current_char, char last_char, uint32_t interval) {
    if (current_char == last_char) {
        if (interval > Interval) {
            return same_char_long;
        } else {
            return same_char_short;
        }
    } else {
        return diff_char;
    }
}

// Finite State Table for Transitions
fsm_transition_t fsm_table[2][3] = {
    // OFF state transitions
    [OFF][same_char_long] = { ON, check_event },
    [OFF][diff_char] = { OFF, check_event },
    [OFF][same_char_short] = { OFF, check_event },

    // ON state transitions
    [ON][same_char_long] = { ON, check_event },
    [ON][diff_char] = { OFF, check_event },
    [ON][same_char_short] = { ON, check_event }
};

// Main FSM Execution Function
enum state fsm_execute(enum state current_state, char current_char, char last_char, uint32_t interval) {
    event event = check_event(current_char, last_char, interval);
    return fsm_table[current_state][event].next_state;
}

int main() {
    stdio_init_all();
    uart_init_setup();

    enum state current_state = OFF;
    char received_char;
    
    while (true) {
        if (is_char_received(&received_char)) {
            // Execute the FSM transition based on the event
            current_state = fsm_execute(current_state, received_char, last_char, counter);
            
            // If the state changes, reset the counter
            if (current_state == OFF) {
                counter = 0;
            }
            last_char = received_char;
        }

        // Reset state if no new input for 500 ms
        if (counter >= Interval) {
            current_state = OFF;
            counter = 0;
        }
    }
}