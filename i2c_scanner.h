/**
 * @file    i2c_scanner.h
 * @brief   I2C Scanner to detect devices on I2C bus
 */


#ifndef _I2C_SCANNER_H_
#define _I2C_SCANNER_H_

#include <stdint.h>
#include "i2c_regs.h"
#include "i2c.h"
#include "mxc_sys.h"

/**
 * @defgroup i2c I2C
 * @ingroup application
 * @{
 */

/***** Definitions *****/

/* Parameters for I2C  */
#define I2C_MASTER	    	MXC_I2C0
#define I2C_MASTER_IDX		0


/***** Function Prototypes *****/

/**
 * @brief   Scan devices on the I2C bus and prints I2C address
 * @param 	none
 * @returns	none
 **/

void i2c_scanner(void);

/**
 * @brief   check if I2C slave returns ACK
 * @param	i2c:		pointer to I2C register base address of used I2C module
 * 			addr:		@parblock I2C 7-bit Address left aligned, bit 7 to bit 1.
 *                          Only supports 7-bit addressing. LSb of the given address
 *                          will be used as the read/write bit, the @p addr <b>will 
 *                          not be shifted</b>. Used for <em>both master</em> and 
 *                          @em slave transactions.  @endparblock
 * @param	data:		data to be sent to slave (dummy)
 * @returns	0:no Ack found, 1:slave sent Ack
 *
 **/
int I2C_MasterWrite_CheckACK(mxc_i2c_regs_t *i2c, uint8_t addr);


/**@} end of group i2c */
#endif /* _I2C_SCANNER_H_ */