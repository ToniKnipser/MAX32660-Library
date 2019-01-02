#include <stdint.h>
#include <stdio.h>
#include "i2c_regs.h"
#include "i2c.h"
#include "mxc_sys.h"
#include "i2c_lcd.h"
#include "max32660.h"


#define I2C_MASTER 		MXC_I2C0
#define I2C_MASTER_IDX	0

/*======================================================================================
 * LCD software functions
 * I2C master assigned to MXC_I2C0 on MAX32660 due to pin sharing with debug interface
 * of second I2C interface.
 * =====================================================================================
 * */


void I2C_LCD_define(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize)
{
	_addr = lcd_addr;
	_cols = lcd_cols;
	_rows = lcd_rows;
	_charsize = charsize;
	_backlightval = LCD_BACKLIGHT;
}


/*-------------------------------------------------------------------------------
 * I2C_LCD_Init
 *-------------------------------------------------------------------------------*/
int I2C_LCD_Init( void )
{
	int  i2c_slave_addr, error;
	const sys_cfg_i2c_t sys_i2c_cfg = NULL; /* No system specific configuration needed. */

	i2c_slave_addr = _addr;
	printf("\nwrite to I2C with address: %#02x\n",i2c_slave_addr);

	//Setup the I2CM
	I2C_Shutdown(I2C_MASTER);
	if((error = I2C_Init(I2C_MASTER, I2C_FAST_MODE, &sys_i2c_cfg)) != E_NO_ERROR)
	{
	    printf("Error initializing I2C%d.  (Error code = %d)\n", I2C_MASTER_IDX, error);
	    return 1;
	}

	txdata[0] = 0xf7;
	if((error = I2C_MasterWrite(I2C_MASTER, i2c_slave_addr, txdata, 1, 0)) != 1)
		{
		    printf("Error writing %d\n", error);
		    while(1);
		}
	return 0;
}


void I2C_LCD_begin()
{
	I2C_LCD_Init();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (_rows > 1)
	{
		_displayfunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((_charsize != 0) && (_rows == 1))
	{
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands.
	delay_ms(50);

	// Now we pull both RS and R/W low to begin commands
	I2C_LCD_expanderWrite(_backlightval);	// reset expander and turn backlight on (Bit 8 =1)

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	I2C_LCD_write4bits(0x03 << 4);
	delay_ms(5); // wait min 4.1ms

	// second try
	I2C_LCD_write4bits(0x03 << 4);
	delay_ms(5); // wait min 4.1ms

	// third go!
	I2C_LCD_write4bits(0x03 << 4);
	delay_us(150);

	// finally, set to 4-bit interface
	I2C_LCD_write4bits(0x02 << 4);

	// set # lines, font size, etc.
	I2C_LCD_command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	I2C_LCD_display();

	// clear it off
	I2C_LCD_clear();

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	I2C_LCD_command(LCD_ENTRYMODESET | _displaymode);

	I2C_LCD_home();
}



/***** high level commands, for the user! ******************************************/
void I2C_LCD_clear()
{
	I2C_LCD_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delay_ms(2);  // this command takes a long time!
}

void I2C_LCD_home()
{
	I2C_LCD_command(LCD_RETURNHOME);  // set cursor position to zero
	delay_ms(2);  // this command takes a long time!
}

void I2C_LCD_setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > _rows)
	{
		row = _rows-1;    // we count rows starting w/0
	}
	I2C_LCD_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void I2C_LCD_noDisplay()
{
	_displaycontrol &= ~LCD_DISPLAYON;
	I2C_LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void I2C_LCD_display()
{
	_displaycontrol |= LCD_DISPLAYON;
	I2C_LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void I2C_LCD_noCursor()
{
	_displaycontrol &= ~LCD_CURSORON;
	I2C_LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void I2C_LCD_cursor()
{
	_displaycontrol |= LCD_CURSORON;
	I2C_LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void I2C_LCD_noBlink()
{
	_displaycontrol &= ~LCD_BLINKON;
	I2C_LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void I2C_LCD_blink()
{
	_displaycontrol |= LCD_BLINKON;
	I2C_LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void I2C_LCD_scrollDisplayLeft(void)
{
	I2C_LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void I2C_LCD_scrollDisplayRight(void)
{
	I2C_LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void I2C_LCD_leftToRight(void)
{
	_displaymode |= LCD_ENTRYLEFT;
	I2C_LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void I2C_LCD_rightToLeft(void)
{
	_displaymode &= ~LCD_ENTRYLEFT;
	I2C_LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void I2C_LCD_autoscroll(void)
{
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	I2C_LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void I2C_LCD_noAutoscroll(void)
{
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	I2C_LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// fill CGRAM locations with custom characters
void I2C_LCD_createChar(uint8_t location, uint8_t charmap[])
{
	location &= 0x7; // we only have 8 locations 0-7
	I2C_LCD_command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++)
	{
		I2C_LCD_write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void I2C_LCD_noBacklight(void)
{
	_backlightval=LCD_NOBACKLIGHT;
	I2C_LCD_expanderWrite(0);
}

void I2C_LCD_backlight(void)
{
	_backlightval=LCD_BACKLIGHT;
	I2C_LCD_expanderWrite(0);
}

int I2C_LCD_getBacklight()
{
  return _backlightval == LCD_BACKLIGHT;
}




/***** mid level commands, for sending data/cmds ********************************/

void I2C_LCD_command(uint8_t value)
{
	I2C_LCD_send(value, 0);
}

int I2C_LCD_write(uint8_t value) {
	I2C_LCD_send(value, Rs);
	return 1;
}


/***** low level data pushing commands ******************************************/

/*-------------------------------------------------------------------------------
 * I2C_LCD_expanderWrite
 *-------------------------------------------------------------------------------*/
int I2C_LCD_expanderWrite (uint8_t LCD_data)
{
	int  i2c_slave_addr, error;
	
	i2c_slave_addr=_addr;
	txdata[0] = LCD_data | _backlightval;
	if((error = I2C_MasterWrite(I2C_MASTER, i2c_slave_addr, txdata, 1, 0)) != 1)
		{
		    printf("Error writing %d\n", error);
		    while(1);
		}
	return 0;
}


// write either command or data
void I2C_LCD_send(uint8_t value, uint8_t mode)
{
	uint8_t highnib=value & 0xf0;
	uint8_t lownib=(value<<4) & 0xf0;
	I2C_LCD_write4bits((highnib)|mode);
	I2C_LCD_write4bits((lownib)|mode);
}

void I2C_LCD_write4bits(uint8_t value)
{
	I2C_LCD_expanderWrite(value);
	I2C_LCD_pulseEnable(value);
}

void I2C_LCD_pulseEnable(uint8_t _data)
{
	I2C_LCD_expanderWrite(_data | En);	// En high
	delay_us(1);		// enable pulse must be >450ns

	I2C_LCD_expanderWrite(_data & ~En);	// En low
	delay_us(50);		// commands need > 37us to settle
}

void I2C_LCD_setBacklight(uint8_t new_val)
{
	if (new_val)
	{
		I2C_LCD_expanderWrite( 0xff); // switch on backlight
	}
	else
	{
		I2C_LCD_expanderWrite( 0xf7); // switch off backlight
	}
}


void I2C_LCD_load_custom_character(uint8_t char_num, uint8_t *rows)
{
	I2C_LCD_createChar(char_num, rows);
}

void I2C_LCD_printstr(const char textstr[])
{
	while( *textstr )
	{
		I2C_LCD_write(*textstr);
		textstr++;
	}
}
