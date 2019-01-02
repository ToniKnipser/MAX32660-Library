
#include <stdint.h>
#include <stdio.h>
#include "i2c_regs.h"
#include "i2c.h"
#include "mxc_sys.h"
#include "max32660.h"



/*-------------------------------------------------------------------------------*/
// SYSTICK_Delay
/*-------------------------------------------------------------------------------*/
void SYSTICK_Delay(unsigned int SYSTICK_nbr)
{
	unsigned int SYSTICK_counter_end;

	SYSTICK_counter_end=SYSTICK_counter+SYSTICK_nbr;
	while(SYSTICK_counter_end > SYSTICK_counter){}
}

/*-------------------------------------------------------------------------------*/
// delay_ms
/*-------------------------------------------------------------------------------*/
void delay_ms(uint32_t t) 
{
  volatile uint32_t elapsed;
  volatile uint32_t start;

  start = SYSTICK_counter;
  do {
    elapsed = SYSTICK_counter - start;
  } while (elapsed < t) ;
}

/*-------------------------------------------------------------------------------
 * delay_us
 *-------------------------------------------------------------------------------*/
void delay_us (unsigned int t)
{
  unsigned int i;

  for(i=0; i<t; i++ ) SYS_SysTick_Delay(96);
}
