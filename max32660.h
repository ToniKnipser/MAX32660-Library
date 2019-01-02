/**
 * @file    max32660.h
 * @brief   some initialization functions
 */


#ifndef _MAX32660_H_
#define _MAX32660_H_

#include <stdint.h>
#include "i2c_regs.h"
#include "i2c.h"
#include "mxc_sys.h"




/**
 * @defgroup i2c I2C
 * @ingroup application
 * @{
 */

/* Variables for SYSTICK */
volatile unsigned int SYSTICK_counter;

/***** Definitions *****/
#define cli() __disable_irq()
#define sei() __enable_irq()


extern volatile unsigned int SYSTICK_counter;


/**
 * @brief   tbd
 * @param	tbd:		
 * @param	tbd:
 * @returns	tbd
 *
 **/
void SYSTICK_Delay(unsigned int );

/**
 * @brief   tbd
 * @returns	none
 *
 **/
void delay_ms(uint32_t);

/**
 * @brief   tbd
 * @returns	none
 **/
void delay_us (unsigned int);

/**@} end of group max32660 */
#endif /* _MAX32660_H_ */
