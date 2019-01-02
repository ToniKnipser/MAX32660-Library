/* button.h
 * buttons provides interrupt handler and functions for elaborating actions on buttons such as 
 * Enter, Back, Next, Previous
 * those buttons are low-side keys connected from input port to gnd. Pull-up should be configured.
 * Low level at input port means button is pressed.
 * All keys must be connected to same I/O port and will be evaluated at the same time.
 * API provided:
 *  Handler with key-debouncing, must be called regularly, e.g. every 10ms
 * 	button activation interfacing
 */
#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include "gpio.h"

/* Variables for Keyboard */
/* keyboard handler is embedded in SysTick_Handler and called every 8ms
 * keyboard functions elaborates on changes in the global variables
 */
typedef struct Button_st Buttons;
struct Button_st{
	uint32_t key_state; 		// debounced and inverted key state: bit = 1: key pressed
	uint32_t key_press;			// key press detect
	uint32_t key_release;		// key release detect
	uint32_t key_rpt;           // key long press and repeat
	uint32_t REPEAT_MASK;
	uint32_t REPEAT_START;
	uint32_t REPEAT_NEXT;
};


void Button_Handler (const gpio_cfg_t *gpio_keys);
uint32_t get_key_press( uint32_t key_mask );
uint32_t get_key_release( uint32_t key_mask );
uint32_t get_key_rpt( uint32_t key_mask );
uint32_t get_key_short( uint32_t key_mask );
uint32_t get_key_long( uint32_t key_mask );


#endif //BUTTON_H
