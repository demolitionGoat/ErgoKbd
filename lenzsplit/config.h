// Copyright 2022 Lorenz Ehlers (@demolitionGoat)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 8 
#define MATRIX_COLS 6

/*
Keyboard Matrix Assignments
*/
#define DIRECT_PINS { {D4,B6,B2,B3,B1,F7},{D4,B6,B2,B3,B1,F7},{D4,B6,B2,B3,B1,F7},{E6,B4,B5,NO_PIN,NO_PIN,NO_PIN} }
#define MATRIX_ROW_PINS { F4,F5,F6,NO_PIN }
#define MATRIX_COL_PINS { D4,B6,B2,B3,B1,F7 }

/*
Encoders
*/
#define ENCODERS_PAD_A { C6 }
#define ENCODERS_PAD_B { D7 }
#define ENCODER_RESOLUTION 2

/*
 * Split Keyboard specific options, make sure you have 'SPLIT_KEYBOARD = yes' in your rules.mk, and define SOFT_SERIAL_PIN.
 */
 
#define MASTER_LEFT
//#define SPLIT_USB_DETECT
#define SOFT_SERIAL_PIN D1 // or D0

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
