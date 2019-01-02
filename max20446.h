#ifndef MAX20446_H
#define MAX20446_H

#include "MAX20446_regs.h"

#define I2C_MAX20446_ADDR 		0xc0  // 0x60h = 7bit address


/* Variables for I2C */
// extern i2c_req_t req;
extern uint8_t txdata[32];
extern uint8_t rxdata[32];
extern volatile int i2c_flag;
extern volatile int i2c_flag1;

int I2C_MAX20446_init( void );
int I2C_MAX20446_read_regfile( void );
uint8_t I2C_MAX20446_read( uint8_t );
int I2C_MAX20446_write( uint8_t, uint8_t );

int I2C_MAX20446_write_PWM ( volatile struct MAX20446_data *);
int I2C_MAX20446_write_regfile( volatile struct MAX20446_data *);

void MAX20446_upload_datafile( volatile struct MAX20446_data *, uint8_t * );
void MAX20446_download_datafile(uint8_t* , volatile struct MAX20446_data *);
void MAX20446_print_datafile( volatile struct MAX20446_data *);

void MAX20446_update_PWM_0pcent (volatile struct MAX20446_data *);
void MAX20446_update_PWM_01pcent (volatile struct MAX20446_data *);
void MAX20446_update_PWM_1pcent (volatile struct MAX20446_data * );
void MAX20446_update_PWM_10pcent (volatile struct MAX20446_data* );
void MAX20446_update_PWM_25pcent (volatile struct MAX20446_data* );
void MAX20446_update_PWM_50pcent (volatile struct MAX20446_data* );

void I2C_MAX20446_start( volatile struct MAX20446_data *);

#endif // MAX20446_H
