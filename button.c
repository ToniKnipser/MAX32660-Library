/** 
 * @brief  Button elaboration routines
 * @param  IRQn  interrupt number that specifies the interrupt
 * @return none.
 * Enable the specified interrupt in the NVIC Interrupt Controller.
 * Other settings of the interrupt such as priority are not affected.
 */

#include "button.h"
#include "gpio.h"
#include "max32660.h"

extern Buttons Debounced;

// keyboard / key detection and debouncing handler
void Button_Handler (const gpio_cfg_t *gpio_keys)
{
	static uint32_t ct0 = 0xFFFFFFFF, ct1 = 0xFFFFFFFF, rpt;
	uint32_t i;

   	i=Debounced.key_state ^ ~(GPIO_InGet(gpio_keys));
   	ct0 = ~( ct0 & i );                         // reset or count ct0
   	ct1 = ct0 ^ (ct1 & i);                      // reset or count ct1
   	i &= ct0 & ct1;                             // count until roll over ?
   	Debounced.key_state ^= i;                             // then toggle debounced state
   	Debounced.key_press |= Debounced.key_state & i;                 // 0->1: key press detect
   	Debounced.key_release |= ~Debounced.key_state & i; 				// 1->0: key release detect

   	if( (Debounced.key_state & Debounced.REPEAT_MASK) == 0 )        // check repeat function
   	     rpt = Debounced.REPEAT_START;                    // start delay
   	if( --rpt == 0 )
   	{
   	    rpt = Debounced.REPEAT_NEXT;                          // repeat delay
   	    Debounced.key_rpt |= Debounced.key_state & Debounced.REPEAT_MASK;
   	}
}

/*-------------------------------------------------------------------------------*/
// read debounced input keys
/*-------------------------------------------------------------------------------*/
uint32_t get_key_press( uint32_t key_mask )
{
  cli();                    // disable Master Interrupt
  key_mask &= Debounced.key_press;    // read key(s)
  Debounced.key_press ^= key_mask;    // clear key(s)
  sei();              		// enable Master Interrupt
  return key_mask;
}

uint32_t get_key_release( uint32_t key_mask )
{
  cli();                     // read and clear atomic !
  key_mask &= Debounced.key_release;   // read key(s)
  Debounced.key_release ^= key_mask;   // clear key(s)
  sei();
  return key_mask;
}

uint32_t get_key_rpt( uint32_t key_mask )
{
  cli();                    // Master Interrupts sperren
  key_mask &= Debounced.key_rpt;              // read key(s)
  Debounced.key_rpt ^= key_mask;               // clear key(s)
  sei();              // Master Interrupts freigeben
  return key_mask;
}


uint32_t get_key_short( uint32_t key_mask )
{
  cli();      // disable Master Interrupts, will be enabled in get_key_press
  return get_key_press( ~Debounced.key_state & key_mask );
}


uint32_t get_key_long( uint32_t key_mask )
{
  return get_key_press( get_key_rpt( key_mask ));
}

