#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"
// split kb stuff
#include "split_common/split_util.h"
#include "split_common/transactions.h"

#define ROWS_PER_HAND (MATRIX_ROWS / 2)
static pin_t direct_pins[ROWS_PER_HAND][MATRIX_COLS] 	= DIRECT_PINS;
static pin_t row_pins[ROWS_PER_HAND] 					= MATRIX_ROW_PINS;
static pin_t col_pins[MATRIX_COLS]   					= MATRIX_COL_PINS;

extern matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values
extern matrix_row_t matrix[MATRIX_ROWS];     // debounced values

extern uint8_t thisHand, thatHand;

static inline void setPinOutput_writeHigh(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinHigh(pin);
    }
}

static inline void setPinInputHigh_atomic(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinInputHigh(pin);
    }
}

static void unselect_col(uint8_t col) {
    pin_t pin = col_pins[col];
    if (pin != NO_PIN) {
#            ifdef MATRIX_UNSELECT_DRIVE_HIGH
        setPinOutput_writeHigh(pin);
#            else
        setPinInputHigh_atomic(pin);
#            endif
    }
}

static void unselect_cols(void) {
    for (uint8_t x = 0; x < MATRIX_COLS; x++) {
        unselect_col(x);
    }
}

static void unselect_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
#            ifdef MATRIX_UNSELECT_DRIVE_HIGH
        setPinOutput_writeHigh(pin);
#            else
        setPinInputHigh_atomic(pin);
#            endif
    }
}

static void init_pins(void) {
	unselect_cols();
    for (uint8_t row = 0; row < ROWS_PER_HAND; row++) {
        if (row_pins[row] != NO_PIN) {
            setPinInputHigh_atomic(row_pins[row]);
        } else {
			for (int col = 0; col < MATRIX_COLS; col++) {
            pin_t pin = direct_pins[row][col];
            if (pin != NO_PIN) {
                setPinInputHigh(pin);
            }
        }
		}
    }
}

static inline uint8_t readMatrixPin(pin_t pin) {
    if (pin != NO_PIN) {
        return readPin(pin);
    } else {
        return 1;
    }
}

static inline void setPinOutput_writeLow(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinLow(pin);
    }
}

static bool select_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
        setPinOutput_writeLow(pin);
        return true;
    }
    return false;
}

static void read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    if (!select_row(current_row)) { // Select row, if no pin scan direct pins
		// Start with a clear matrix row
		matrix_row_t current_row_value = 0;
	
		matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
		for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
			pin_t pin = direct_pins[current_row][col_index];
			if (pin != NO_PIN) {
				current_row_value |= readPin(pin) ? 0 : row_shifter;
			}
		}
	
		// Update the matrix
		current_matrix[current_row] = current_row_value;
		return;                     // return after direct pin row
    }
    matrix_output_select_delay();

    // For each col...
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        uint8_t pin_state = readMatrixPin(col_pins[col_index]);

        // Populate the matrix row with the state of the col pin
        current_row_value |= pin_state ? 0 : row_shifter;
    }

    // Unselect row
    unselect_row(current_row);
    matrix_output_unselect_delay(current_row, current_row_value != 0); // wait for all Col signals to go HIGH

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}

void matrix_init_custom(void) {
    // TODO: initialize hardware here
	init_pins();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
	matrix_row_t curr_matrix[MATRIX_ROWS] = {0};

	// Set row, read cols
	for (uint8_t current_row = 0; current_row < ROWS_PER_HAND; current_row++) {
		read_cols_on_row(curr_matrix, current_row);
	}

	bool changed = memcmp(raw_matrix, curr_matrix, sizeof(curr_matrix)) != 0;
	if (changed) memcpy(raw_matrix, curr_matrix, sizeof(curr_matrix));

	changed = debounce(raw_matrix, matrix + thisHand, ROWS_PER_HAND, changed) | matrix_post_scan();

    return (uint8_t)changed;
}
