
#include <stdio.h>
#include <stdint.h>

#include "mxc_sys.h"
#include "max20446.h"
#include "max20446_regs.h"
#include "i2c.h"
#include "max32660.h"
#include "max32660_config.h"

extern volatile struct MAX20446_data MAX20446reg;

/*-------------------------------------------------------------------------------
 * I2C_MAX20446_init
 *-------------------------------------------------------------------------------*/
int I2C_MAX20446_init (void)
{
	int  error;
	uint8_t device_id, device_rev;
	const sys_cfg_i2c_t sys_i2c_cfg = NULL; /* No system specific configuration needed. */

	//Setup the I2CM
	I2C_Shutdown(I2C_MASTER);
	if((error = I2C_Init(I2C_MASTER, I2C_FAST_MODE, &sys_i2c_cfg)) != E_NO_ERROR)
	{
	    printf("Error initializing I2C%d.  (Error code = %d)\n", I2C_MASTER_IDX, error);
	    return 1;
	}
	printf("I2C Master initialized successfully\n");
	// read ID
	device_id = I2C_MAX20446_read (0);
	printf("MAX20046 ID = %#04x\n", device_id);
	device_rev = I2C_MAX20446_read (1);
	printf("MAX20046 Revision = %d\n", device_rev);
	return 0;
}


/*-------------------------------------------------------------------------------
 * I2C_MAX20446_read_regfile
 *-------------------------------------------------------------------------------*/
int I2C_MAX20446_read_regfile (void)
{
	int  i2c_slave_addr, error, i, j;

	i2c_slave_addr = I2C_MAX20446_ADDR;

	printf("MAX20446: read register file from I2C address: %#02x\n",i2c_slave_addr);

	//printf("Master writes Data pointer to Slave.\n");
	txdata[0] = 0;
	if((error = I2C_MasterWrite(I2C_MASTER, i2c_slave_addr, txdata, 1, 0)) != 1)
	{
	    printf("Error writing %d\n", error);
	    while(1);
	}

	//printf("Master reads 32 bytes of data from Slave.\n");
	if((error = I2C_MasterRead(I2C_MASTER, i2c_slave_addr, rxdata, 32, 0)) != 32)
	{
		printf("Error reading%d\n", error);
		while(1);
	}

	// print data
	for(j = 0; j < 32; j+=8)
	{
		printf("\033[33m%#04x\033[0m ",j);							//address
		for(i = j; i < j+8; i++)
		    {
		        printf("\t%#04x", rxdata[i]);
		    }
		    printf("\n");
	}
	return 0;
}

/*-------------------------------------------------------------------------------
 * I2C_MAX20446_read
 *-------------------------------------------------------------------------------*/
uint8_t I2C_MAX20446_read (uint8_t MAX20466_regaddr)
{
	int  i2c_slave_addr, error;

	i2c_slave_addr = I2C_MAX20446_ADDR;

	printf("MAX20446: read databyte from I2C address: %#02x\n",i2c_slave_addr);

	//printf("Master writes Data pointer to Slave.\n");
	txdata[0] = MAX20466_regaddr;
	if((error = I2C_MasterWrite(I2C_MASTER, i2c_slave_addr, txdata, 1, 0)) != 1)
	{
	    printf("Error writing %d\n", error);
	    while(1);
	}

	if((error = I2C_MasterRead(I2C_MASTER, i2c_slave_addr, rxdata, 1, 0)) != 1)
	{
		printf("Error reading%d\n", error);
		while(1);
	}

	// print data
		printf("\033[33m%#04x\033[0m ",MAX20466_regaddr);							//address
        printf("\t%#04x\n", rxdata[0]);

	return rxdata[0];
}



/*-------------------------------------------------------------------------------
 * I2C_MAX20446_write
 *-------------------------------------------------------------------------------*/
int I2C_MAX20446_write (uint8_t MAX20466_regaddr, uint8_t MAX20466_databyte)
{
	int  i2c_slave_addr, error;

	i2c_slave_addr = I2C_MAX20446_ADDR;

	printf("MAX20446: write databyte: %#02x to register: %#02x\n", MAX20466_databyte, MAX20466_regaddr);

	//printf("Master writes Data pointer to Slave.\n");
		txdata[0] = MAX20466_regaddr;
		txdata[1] = MAX20466_databyte;
		if((error = I2C_MasterWrite(I2C_MASTER, i2c_slave_addr, txdata, 2, 0)) != 2)
		{
		    printf("Error writing %d\n", error);
		    while(1);
		}

	return 0;
}


/*-------------------------------------------------------------------------------
 * I2C_MAX20446_write_PWM
 *-------------------------------------------------------------------------------*/
int I2C_MAX20446_write_PWM (volatile struct MAX20446_data *MAX20446_data_ptr)
{
	int  i2c_slave_addr, error;
	volatile uint32_t dummyI;

	i2c_slave_addr = I2C_MAX20446_ADDR;

	printf("[%#06d] %s %s %s", SYSTICK_counter,
			"\033[32mINFO\033[0m",
			"I2C_MAX20446_write_PWM:",
			"update PWM register map\n");

	//printf("Master writes Data pointer to Slave.\n");
		txdata[0] = MAX20446_I2C_TONH1;	// start address of PWM register map

		//TONH1 Register
		txdata[1] = ((MAX20446_data_ptr->mPWM1) >> 10) & 0xff;
		//TONL1 Register
		txdata[2] = ((MAX20446_data_ptr->mPWM1) >> 2) & 0xff;
		//TONH2 Register
		txdata[3] = ((MAX20446_data_ptr->mPWM2) >> 10) & 0xff;
		//TONL2 Register
		txdata[4] = ((MAX20446_data_ptr->mPWM2) >> 2) & 0xff;
		//TONH3 Register
		txdata[5] = ((MAX20446_data_ptr->mPWM3) >> 10) & 0xff;
		//TONL3 Register
		txdata[6] = ((MAX20446_data_ptr->mPWM3) >> 2) & 0xff;
		//TONH4 Register
		txdata[7] = ((MAX20446_data_ptr->mPWM4) >> 10) & 0xff;
		//TONL4 Register
		txdata[8] = ((MAX20446_data_ptr->mPWM4) >> 2) & 0xff;
		//TONL1_4 Register
		dummyI = ((MAX20446_data_ptr->mPWM4) & 0x3);
		dummyI = dummyI << 2;
		dummyI |= ((MAX20446_data_ptr->mPWM3) & 0x3);
		dummyI = (dummyI << 2);
		dummyI |= ((MAX20446_data_ptr->mPWM2) & 0x3);
		dummyI = (dummyI << 2);
		dummyI |= ((MAX20446_data_ptr->mPWM1) & 0x3);
		txdata[9] = (uint8_t) dummyI;
		//TONH5 Register
		txdata[10] = ((MAX20446_data_ptr->mPWM5) >> 10) & 0xff;
		//TONL5 Register
		txdata[11] = ((MAX20446_data_ptr->mPWM5) >> 2) & 0xff;
		//TONH6 Register
		txdata[12] = ((MAX20446_data_ptr->mPWM6) >> 10) & 0xff;
		//TONL6 Register
		txdata[13] = ((MAX20446_data_ptr->mPWM6) >> 2) & 0xff;
		//TONL5_6 Register
		dummyI = ((MAX20446_data_ptr->mPWM6) & 0x3);
		dummyI = dummyI << 2;
		dummyI |= ((MAX20446_data_ptr->mPWM5) & 0x3);
		txdata[14] = (uint8_t) dummyI;

		if((error = I2C_MasterWrite(I2C_MASTER, i2c_slave_addr, txdata, 15, 0)) != 15)
		{
		    printf("Error writing %d\n", error);
		    while(1);
		}

	return 0;
}

/*-------------------------------------------------------------------------------
 * I2C_MAX20446_write_regfile
 *-------------------------------------------------------------------------------*/
int I2C_MAX20446_write_regfile (volatile struct MAX20446_data *MAX20446_data_ptr)
{
	int  i2c_slave_addr, error;


	i2c_slave_addr = I2C_MAX20446_ADDR;

	printf("MAX20446: write register file to I2C address: %#02x\n",i2c_slave_addr);

	//printf("Master writes Data pointer to Slave.\n");
		txdata[0] = 0x02;	// data write starts at address 0x02
		MAX20446_upload_datafile( MAX20446_data_ptr, txdata+1);
		if((error = I2C_MasterWrite(MXC_I2C0, i2c_slave_addr, txdata, 19, 0)) != 19)
		{
		    printf("Error writing %d\n", error);
		    while(1);
		}

	return 0;
}


/*-------------------------------------------------------------------------------
 * MAX20446_upload_datafile
 *
 *
 *-------------------------------------------------------------------------------*/
void MAX20446_upload_datafile( volatile struct MAX20446_data* MAX20446_data_ptr, uint8_t* dest )
{
	uint8_t dummyB;
	uint32_t dummyI;

	//ISET Register
	dummyB = ((MAX20446_data_ptr->mISET) & 0x0f) | ((MAX20446_data_ptr->mStatus) & 0xf0);
	*dest++ = dummyB;

	//IMODE Register
	dummyB = (MAX20446_data_ptr->mStatus) & 0x0f;
	*dest++ = dummyB;

	//TONH1 Register
	dummyI = ((MAX20446_data_ptr->mPWM1) >> 10) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONL1 Register
	dummyI = ((MAX20446_data_ptr->mPWM1) >> 2) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONH2 Register
	dummyI = ((MAX20446_data_ptr->mPWM2) >> 10) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONL2 Register
	dummyI = ((MAX20446_data_ptr->mPWM2) >> 2) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONH3 Register
	dummyI = ((MAX20446_data_ptr->mPWM3) >> 10) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONL3 Register
	dummyI = ((MAX20446_data_ptr->mPWM3) >> 2) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONH4 Register
	dummyI = ((MAX20446_data_ptr->mPWM4) >> 10) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONL4 Register
	dummyI = ((MAX20446_data_ptr->mPWM4) >> 2) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONL1_4 Register
	dummyI = ((MAX20446_data_ptr->mPWM4) & 0x3);
	dummyI = dummyI << 2;
	dummyI |= ((MAX20446_data_ptr->mPWM3) & 0x3);
	dummyI = (dummyI << 2);
	dummyI |= ((MAX20446_data_ptr->mPWM2) & 0x3);
	dummyI = (dummyI << 2);
	dummyI |= ((MAX20446_data_ptr->mPWM1) & 0x3);
	*dest++ = (uint8_t) dummyI;

	//TONH5 Register
	dummyI = ((MAX20446_data_ptr->mPWM5) >> 10) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONL5 Register
	dummyI = ((MAX20446_data_ptr->mPWM5) >> 2) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONH6 Register
	dummyI = ((MAX20446_data_ptr->mPWM6) >> 10) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONL6 Register
	dummyI = ((MAX20446_data_ptr->mPWM6) >> 2) & 0xff;
	*dest++ = (uint8_t) dummyI;

	//TONL5_6 Register
	dummyI = ((MAX20446_data_ptr->mPWM6) & 0x3);
	dummyI = dummyI << 2;
	dummyI |= ((MAX20446_data_ptr->mPWM5) & 0x3);
	*dest++ = (uint8_t) dummyI;

	//Setting Register
	dummyB = (MAX20446_data_ptr->mSetting) & 0x7f;
	*dest++ =  dummyB;

	//Disable Register
	dummyB = (MAX20446_data_ptr->mDisable) & 0x3f;
	*dest++ =  dummyB;

	//BSTMON Register and following are all read only
}

/*-------------------------------------------------------------------------------
 * MAX20446_download_datafile
 *-------------------------------------------------------------------------------*/
void MAX20446_download_datafile(uint8_t* source, volatile struct MAX20446_data *MAX20446_data_ptr)
{
	uint8_t dummyB;
	uint32_t dummyI;

	MAX20446_data_ptr->mDev_ID = *source++;
	dummyB=*source++;
	MAX20446_data_ptr->mRev_ID = dummyB & 0x0f;
	MAX20446_data_ptr->mLoDIM = (dummyB & ~0x3);
	dummyB=*source++;
	MAX20446_data_ptr->mISET = (dummyB & 0x0f);
	MAX20446_data_ptr->mStatus = (dummyB & 0x70);
	dummyB=*source++;
	MAX20446_data_ptr->mStatus |= (dummyB & 0x0f);
	MAX20446_data_ptr->mLoDIM |= (dummyB & 0xf0) >> 4;
	dummyI=*source++;
	MAX20446_data_ptr->mPWM1 = (dummyI << 10);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM1 |= (dummyI << 2);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM2 = (dummyI << 10);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM2 |= (dummyI << 2);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM3 = (dummyI << 10);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM3 |= (dummyI << 2);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM4 = (dummyI << 10);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM4 |= (dummyI << 2);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM1 |= (dummyI & 0x03);
	MAX20446_data_ptr->mPWM2 |= (dummyI & 0x0c) >> 2;
	MAX20446_data_ptr->mPWM3 |= (dummyI & 0x30) >> 4;
	MAX20446_data_ptr->mPWM4 |= (dummyI & 0xc0) >> 6;
	dummyI=*source++;
	MAX20446_data_ptr->mPWM5 = (dummyI << 10);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM5 |= (dummyI << 2);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM6 = (dummyI << 10);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM6 |= (dummyI << 2);
	dummyI=*source++;
	MAX20446_data_ptr->mPWM5 |= (dummyI & 0x03);
	MAX20446_data_ptr->mPWM6 |= (dummyI & 0x0c) >> 2;
	dummyB=*source++;
	MAX20446_data_ptr->mSetting = (dummyB & 0x7f);
	dummyB=*source++;
	MAX20446_data_ptr->mDisable = (dummyB & 0x3f);
	MAX20446_data_ptr->mVMON = *source++;
	MAX20446_data_ptr->mIOUT1 = *source++;
	MAX20446_data_ptr->mIOUT2 = *source++;
	MAX20446_data_ptr->mIOUT3 = *source++;
	MAX20446_data_ptr->mIOUT4 = *source++;
	MAX20446_data_ptr->mIOUT5 = *source++;
	MAX20446_data_ptr->mIOUT6 = *source++;
	MAX20446_data_ptr->mOpen = *source++;
	MAX20446_data_ptr->mShortgnd = *source++;
	MAX20446_data_ptr->mShortled = *source++;
	MAX20446_data_ptr->mMask = *source++;
	MAX20446_data_ptr->mDiag = *source;
}

/*-------------------------------------------------------------------------------
 * MAX20446_print_datafile
 *-------------------------------------------------------------------------------*/
void MAX20446_print_datafile( volatile struct MAX20446_data *MAX20446_data_ptr)
{
	printf("MAX20446: ID = %#04x\n", MAX20446_data_ptr->mDev_ID);
	printf("MAX20446: Rev = %#04x\n", MAX20446_data_ptr->mRev_ID);
	printf("MAX20446: ISET = %#04x\n", MAX20446_data_ptr->mISET);
	printf("MAX20446: LoDIM = %#04x\n", MAX20446_data_ptr->mLoDIM);
	printf("MAX20446: Status flags = %#04x\n", MAX20446_data_ptr->mStatus);
	printf("MAX20446: PWM1 = %#07x\n", MAX20446_data_ptr->mPWM1);
	printf("MAX20446: PWM2 = %#07x\n", MAX20446_data_ptr->mPWM2);
	printf("MAX20446: PWM3 = %#07x\n", MAX20446_data_ptr->mPWM3);
	printf("MAX20446: PWM4 = %#07x\n", MAX20446_data_ptr->mPWM4);
	printf("MAX20446: PWM5 = %#07x\n", MAX20446_data_ptr->mPWM5);
	printf("MAX20446: PWM6 = %#07x\n", MAX20446_data_ptr->mPWM6);
	printf("MAX20446: Setting = %#04x\n", MAX20446_data_ptr->mSetting);
	printf("MAX20446: Disable = %#04x\n", MAX20446_data_ptr->mDisable);
	printf("MAX20446: VMON = %#04x\n", MAX20446_data_ptr->mVMON);
	printf("MAX20446: IOUT1 = %#04x\n", MAX20446_data_ptr->mIOUT1);
	printf("MAX20446: IOUT2 = %#04x\n", MAX20446_data_ptr->mIOUT2);
	printf("MAX20446: IOUT3 = %#04x\n", MAX20446_data_ptr->mIOUT3);
	printf("MAX20446: IOUT4 = %#04x\n", MAX20446_data_ptr->mIOUT4);
	printf("MAX20446: IOUT5 = %#04x\n", MAX20446_data_ptr->mIOUT5);
	printf("MAX20446: IOUT6 = %#04x\n", MAX20446_data_ptr->mIOUT6);
	printf("MAX20446: OPEN = %#04x\n", MAX20446_data_ptr->mOpen);
	printf("MAX20446: Short GND = %#04x\n", MAX20446_data_ptr->mShortgnd);
	printf("MAX20446: Short LED = %#04x\n", MAX20446_data_ptr->mShortled);
	printf("MAX20446: MasK = %#04x\n", MAX20446_data_ptr->mMask);
	printf("MAX20446: Diag = %#04x\n", MAX20446_data_ptr->mDiag);
}



void MAX20446_update_PWM_0pcent (volatile struct MAX20446_data* MAX20446_data_ptr )
{
	MAX20446reg.mPWM1 = 000;		// at 200Hz(5ms) 50.000ns=50%, 25.000=25%, 10.000=10%, 1.000=1%
	MAX20446reg.mPWM2 = 000;
	MAX20446reg.mPWM3 = 000;
	MAX20446reg.mPWM4 = 000;
	MAX20446reg.mPWM5 = 000;
	MAX20446reg.mPWM6 = 000;
}

void MAX20446_update_PWM_01pcent (volatile struct MAX20446_data* MAX20446_data_ptr )
{
	MAX20446reg.mPWM1 = 100;		// at 200Hz(5ms) 50.000ns=50%, 25.000=25%, 10.000=10%, 1.000=1%
	MAX20446reg.mPWM2 = 100;
	MAX20446reg.mPWM3 = 100;
	MAX20446reg.mPWM4 = 100;
	MAX20446reg.mPWM5 = 100;
	MAX20446reg.mPWM6 = 100;
}

void MAX20446_update_PWM_1pcent (volatile struct MAX20446_data* MAX20446_data_ptr )
{
	MAX20446reg.mPWM1 = 1000;		// at 200Hz(5ms) 50.000ns=50%, 25.000=25%, 10.000=10%, 1.000=1%
	MAX20446reg.mPWM2 = 1000;
	MAX20446reg.mPWM3 = 1000;
	MAX20446reg.mPWM4 = 1000;
	MAX20446reg.mPWM5 = 1000;
	MAX20446reg.mPWM6 = 1000;
}

void MAX20446_update_PWM_10pcent (volatile struct MAX20446_data* MAX20446_data_ptr )
{
	MAX20446reg.mPWM1 = 10000;		// at 200Hz(5ms) 50.000ns=50%, 25.000=25%, 10.000=10%, 1.000=1%
	MAX20446reg.mPWM2 = 10000;
	MAX20446reg.mPWM3 = 10000;
	MAX20446reg.mPWM4 = 10000;
	MAX20446reg.mPWM5 = 10000;
	MAX20446reg.mPWM6 = 10000;
}

void MAX20446_update_PWM_25pcent (volatile struct MAX20446_data* MAX20446_data_ptr )
{
	MAX20446reg.mPWM1 = 25000;		// at 200Hz(5ms) 50.000ns=50%, 25.000=25%, 10.000=10%, 1.000=1%
	MAX20446reg.mPWM2 = 25000;
	MAX20446reg.mPWM3 = 25000;
	MAX20446reg.mPWM4 = 25000;
	MAX20446reg.mPWM5 = 25000;
	MAX20446reg.mPWM6 = 25000;
}

void MAX20446_update_PWM_50pcent (volatile struct MAX20446_data* MAX20446_data_ptr )
{
	MAX20446reg.mPWM1 = 50000;		// at 200Hz(5ms) 50.000ns=50%, 25.000=25%, 10.000=10%, 1.000=1%
	MAX20446reg.mPWM2 = 50000;
	MAX20446reg.mPWM3 = 50000;
	MAX20446reg.mPWM4 = 50000;
	MAX20446reg.mPWM5 = 50000;
	MAX20446reg.mPWM6 = 50000;
}






void I2C_MAX20446_start( volatile struct MAX20446_data *MAX20446_data_ptr )
{
	uint8_t  MAX20466_regdata;

	I2C_MAX20446_read_regfile();
	MAX20446_download_datafile( rxdata, MAX20446_data_ptr );
	MAX20446_print_datafile( MAX20446_data_ptr );

	MAX20446reg.mPWM1 = 1000;		// at 200Hz(5ms) 50.000ns=50%, 25.000=25%, 10.000=10%, 1.000=1%
	MAX20446reg.mPWM2 = 1000;
	MAX20446reg.mPWM3 = 1000;
	MAX20446reg.mPWM4 = 1000;
	MAX20446reg.mPWM5 = 1000;
	MAX20446reg.mPWM6 = 1000;
	MAX20446reg.mISET = 0x01;		// 50mA LED current per channel (0b=100mA
	MAX20446reg.mStatus = 0;		// internal dimming
	MAX20446reg.mSetting = 0x1a; 	// 205 Hz PWM, 6V LED short, SS disabled
	MAX20446reg.mDisable = 0x00;	// 0= all channels are enabled, 0x38=disable channel 4,5,6

	MAX20446_upload_datafile( MAX20446_data_ptr, txdata );
	I2C_MAX20446_write_regfile(MAX20446_data_ptr);

	I2C_MAX20446_write (MAX20446_I2C_MASK, 0);	// all failure gets output on fault pin

	MAX20466_regdata= MAX20446reg.mISET | 0x20;	//enable boost
	I2C_MAX20446_write (0x02, MAX20466_regdata);

	I2C_MAX20446_read_regfile();
	MAX20446_download_datafile( rxdata, MAX20446_data_ptr );
	MAX20446_print_datafile( MAX20446_data_ptr );
}

