#ifndef I2C_LCD_H
#define I2C_LCD_H


/* global Variables for I2C */
// extern i2c_req_t req;
uint8_t txdata[32];
uint8_t rxdata[32];
volatile int i2c_flag;
volatile int i2c_flag1;
extern int I2C_LCD_ADDR;  		//LCD with PCF8574 expansion e.g. 0x4e

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

/**
 * This is the driver for the Liquid Crystal LCD displays that use the I2C bus.
 *
 * The backlight is on by default, since that is the most likely operating mode in
 * most cases.
 */
 
/**
Example: 
// Set the LCD address to 0x27 for a 16 chars and 2 line display
	I2C_LCD_define(0x27, 16, 2);
// initialize the LCD
	I2C_LCD_begin();
// Turn on the blacklight and print a message.
	I2C_LCD_backlight();
	I2C_LCD_printstr("Hello, world!");
 */

 
 	/**
	 * 
	 *
	 * @param tbd	
	 */
	int I2C_LCD_Init( void );
 
	/**
	 * 
	 *
	 * @param lcd_addr	I2C slave address of the LCD display. Most likely printed on the
	 *					LCD circuit board, or look in the supplied LCD documentation.
	 * @param lcd_cols	Number of columns your LCD display has.
	 * @param lcd_rows	Number of rows your LCD display has.
	 * @param charsize	The size in dots that the display has, use LCD_5x10DOTS or LCD_5x8DOTS.
	 */
	void I2C_LCD_define(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize );

	/**
	 * Set the LCD display in the correct begin state, must be called before anything else is done.
	 */
	void I2C_LCD_begin();

	 /**
	  * Remove all the characters currently shown. Next print/write operation will start
	  * from the first position on LCD display.
	  */
	void I2C_LCD_clear();

	/**
	 * Next print/write operation will will start from the first position on the LCD display.
	 */
	void I2C_LCD_home();

	 /**
	  * Do not show any characters on the LCD display. Backlight state will remain unchanged.
	  * Also all characters written on the display will return, when the display in enabled again.
	  */
	void I2C_LCD_noDisplay();

	/**
	 * Show the characters on the LCD display, this is the normal behaviour. This method should
	 * only be used after noDisplay() has been used.
	 */
	void I2C_LCD_display();

	/**
	 * Do not blink the cursor indicator.
	 */
	void I2C_LCD_noBlink();

	/**
	 * Start blinking the cursor indicator.
	 */
	void I2C_LCD_blink();

	/**
	 * Do not show a cursor indicator.
	 */
	void I2C_LCD_noCursor();

	/**
 	 * Show a cursor indicator, cursor can blink on not blink. Use the
	 * methods blink() and noBlink() for changing cursor blink.
	 */
	void I2C_LCD_cursor();

	void I2C_LCD_scrollDisplayLeft();
	void I2C_LCD_scrollDisplayRight();
	void I2C_LCD_printLeft();
	void I2C_LCD_printRight();
	void I2C_LCD_leftToRight();
	void I2C_LCD_rightToLeft();
	void I2C_LCD_shiftIncrement();
	void I2C_LCD_shiftDecrement();
	void I2C_LCD_noBacklight();
	void I2C_LCD_backlight();
	int I2C_LCD_getBacklight();
	void I2C_LCD_autoscroll();
	void I2C_LCD_noAutoscroll();
	void I2C_LCD_createChar(uint8_t, uint8_t[]);
	void I2C_LCD_setCursor(uint8_t, uint8_t);
	int I2C_LCD_write(uint8_t);
	void I2C_LCD_command(uint8_t);

	/*
	inline void blink_on() { blink(); }
	inline void blink_off() { noBlink(); }
	inline void cursor_on() { cursor(); }
	inline void cursor_off() { noCursor(); }
*/

// Compatibility API function aliases
	void I2C_LCD_setBacklight(uint8_t new_val);				// alias for backlight() and nobacklight()
	void I2C_LCD_load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
	void I2C_LCD_printstr(const char[]);

// private
	void I2C_LCD_send(uint8_t, uint8_t);
	void I2C_LCD_write4bits(uint8_t);
	int I2C_LCD_expanderWrite (uint8_t LCD_data);
	void I2C_LCD_pulseEnable(uint8_t);
	
	uint8_t _addr;
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
	uint8_t _cols;
	uint8_t _rows;
	uint8_t _charsize;
	uint8_t _backlightval;
	


#endif // I2C_LCD_H
