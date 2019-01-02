
#include <stdint.h>
#include <stdio.h>
#include "i2c_regs.h"
#include "i2c.h"
#include "mxc_sys.h"
#include "i2c_scanner.h"




/*-------------------------------------------------------------------------------*/
//I2C_MasterWrite_CheckACK
/* ------------------------------------------------------------------------------ */
/* modified function for I2C Master to send data and returns I2C error
 * is being used from i2C_scanner to detect whether a specific slave responds
 * with an ACK
 *
 * Parameters:
 * 			*i2c:		I2C base address of used I2C module
 * 			addr:		i2C slave address to be communicated / checked
 * Returns:
 * 			0: 			no Ack found
 * 			1: 			slave sent Ack
 *
 *  Interrupt Flag Register 0 bit 7:
 *  Address ACK from External Slave Interrupt Flag
 *  This field is set when a slave address ACK is received.
 *  Write 1 to clear. Write 0 has no effect.
 *  0: Condition has not occurred.
 *  1: The slave device ACK for the address was received
 *
 * Interrupt Flag Register 0 bit 10:
 *  Address NACK from Slave Error Flag
 *  This flag is set by hardware if an Address NACK is received from a slave on the I2C bus.
 *  This flag is only valid if the I2Cn peripheral is configured for Master Mode operation.
 *  Write 1 to clear. Write 0 has no effect.
 *  0: Error condition has not occurred.
 *  1: Address NACK received from a slave
 *
 * Interrupt Flag Register 0 bit 7 (ACK) is being used to detect attached I2C device.
 *
 * Anton Mühlhofer, 29.Oct 2018
 * -------------------------------------------------------------------------------*/
/**
 * @file    I2C_Scanner.c
 * @brief   Scan devices on the I2C bus and prints found I2C addresses
 * @details Parameters:
 * 			*i2c:		I2C base address of used I2C module
 * 			addr:		i2C slave address to be communicated / checked

 * 			Returns:
 * 			0: 			no Ack found
 * 			1: 			slave sent Ack
 *
 ************************************************************************************/
int I2C_MasterWrite_CheckACK(mxc_i2c_regs_t *i2c, uint8_t addr)
{
    int I2C_isACK=0;

    // Clear the lock out bit (W1C) in case it is set.
    i2c->int_fl0 = MXC_F_I2C_INT_FL0_TX_LOCK_OUT;

    // Enable master mode
    i2c->ctrl |= MXC_F_I2C_CTRL_MST;

    // Load FIFO with slave address for WRITE and dummy data
    while (i2c->status & MXC_F_I2C_STATUS_TX_FULL) {}
    i2c->fifo = addr & ~(0x1);	// slave address
    i2c->fifo = 0x0;			// dummy data

    // start trying sending bytes
    i2c->master_ctrl |= MXC_F_I2C_MASTER_CTRL_START;
    i2c->master_ctrl |= MXC_F_I2C_MASTER_CTRL_STOP;

    // Wait for Done
    while (!(i2c->int_fl0 & MXC_F_I2C_INT_FL0_DONE)) {}
    i2c->int_fl0 = MXC_F_I2C_INT_FL0_DONE; // clear done flag

    // Wait for Stop
    while (!(i2c->int_fl0 & MXC_F_I2C_INT_FL0_STOP)) {}
    i2c->int_fl0 = MXC_F_I2C_INT_FL0_STOP;

    // Check for acknowledge
    I2C_isACK=0;
    if (i2c->int_fl0 & MXC_F_I2C_INT_FL0_ADDR_ACK)
    	I2C_isACK=1;
    i2c->int_fl0 = MXC_F_I2C_INT_FL0_ADDR_ACK; // clear ACK flag

    return I2C_isACK;
}


/*-------------------------------------------------------------------------------*/
//I2C Scanner
/*-------------------------------------------------------------------------------*/
/* main routing to start i2C Scanner
*/
/**
 * @file    I2C_Scanner.c
 * @brief   Scan devices on the I2C bus
 * @details Parameters:
 * 				none
 *			Calls:
 *				I2C_Shutdown
 *				I2C_Init
 *				I2C_MasterWrite_CheckACK
 * 			Returns:
 * 			none
 **/
/*-------------------------------------------------------------------------------*/
void i2c_scanner(void)
{
	int  i2c_test_addr, error, i = 0, nbr_devices_found, ack_found;
	const sys_cfg_i2c_t sys_i2c_cfg = NULL; /* No system specific configuration needed. */

	printf("\n----- I2C Scanner v0.1 -----\n");

 //Setup the I2CM
	I2C_Shutdown(I2C_MASTER);
	if((error = I2C_Init(I2C_MASTER, I2C_FAST_MODE, &sys_i2c_cfg)) != E_NO_ERROR) {
		printf("Error initializing I2C%d.  (Error code = %d)\n", I2C_MASTER_IDX, error);
//		return error;
 }

 printf("Master starts scanning ... \n");

 nbr_devices_found=0;
 for( i = 0; i<128; i++)
 	 {
	 i2c_test_addr=i << 1;
	 ack_found = I2C_MasterWrite_CheckACK(MXC_I2C0, i2c_test_addr);
	 if( ack_found)
		 {
		 printf("I2C: %#04x\n", i2c_test_addr);
		 nbr_devices_found++;
		 }
 	 }
 printf("Test complete. %d devices found\n", nbr_devices_found);
}
