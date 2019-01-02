#include "CharMenu.h"
#include "max32660.h"
#include "i2c_lcd.h"
#include "MyConfig.h"
#include "CharMenu_settings.h"
#include "button.h"

/**
 * Main process for the CharMenu framework.
 * this process will show menus and listen to any button pressed, 
 * and execute each menu selected
 */
void CharMenu__draw(CharMenu *self)
{
	if (self->current_menu==NULL)
	{
		self->current_menu = self->menu_root->child_head;
	}
	I2C_LCD_setCursor(0,1);
	I2C_LCD_printstr(self->current_menu->text);
	self->print_scroll(
				self,
				self->current_menu->pos,
				self->current_menu->parent->children
			);
	uint8_t action = self->button_read(self);
	switch(action) {
		case CHARMENU_BUTTON_ENTER_DOWN:
			//Each Menu should have either action or having children
			if (self->current_menu->action!=NULL)
			{
				self->current_menu->action();//Execution here (will be blocking)
			}
			else if (self->current_menu->child_head!=NULL)
			{
				self->current_menu = self->current_menu->child_head;
			}
		break;
		case CHARMENU_BUTTON_NEXT_DOWN:
			if (self->current_menu->next!=NULL)
			{
				self->current_menu = self->current_menu->next;
			}
			else
			{
				//rotate to the oldest sibling
				while (self->current_menu->prev!=NULL)
					self->current_menu = self->current_menu->prev;
			}
		break;
		case CHARMENU_BUTTON_PREV_DOWN:
			if (self->current_menu->prev!=NULL)
			{
				self->current_menu = self->current_menu->prev;
			}
			else
			{
				//rotate to the youngest sibling
				while (self->current_menu->next!=NULL)
					self->current_menu = self->current_menu->next;
			}
		break;
		case CHARMENU_BUTTON_BACK_DOWN:
			if (self->current_menu->parent!=NULL)
			{
				self->current_menu = self->current_menu->parent;
			}
		break;
	}
	return;
}


void CharMenu__init(CharMenu *self)
{
	
	self->button_wait = &CharMenu__button_wait;
	self->button_is_pressed = &CharMenu__button_is_pressed;
	self->button_read = &CharMenu__button_read;
	self->draw = &CharMenu__draw;
	self->lcd_number = &CharMenu__lcd_number;
	self->print_scroll = &CharMenu__print_scroll;
	self->current_menu = NULL;
	self->menu_root = new_Menu(self->menu_root);
	
	self->menu_root->text = "root:     [Enter";

}

/** Wait for any Button to be pressed */
void CharMenu__button_wait(CharMenu *self)
{
	while (!get_key_press( GPIO_PIN_KEYALL ) ); // wait for short key
}

/** Wait for any Button to be pressed and return which Button is pressed 
 * Warning: delay is removed, when using, manually put delay after function called
 * this is important to give the time the User release the button before any next instruction executed,
 * example:
 *		charmenu->button_read(charmenu);
 * 		_delay_ms(100);
 * */
uint8_t CharMenu__button_read(CharMenu *self)
{
	uint8_t output=0;
	uint32_t readkey;
	
	//self->button_wait(self);	// wait for key pressed
	
	do{
		readkey = get_key_short(GPIO_PIN_KEY1);
		if (readkey){
			output = CHARMENU_BUTTON_PREV_DOWN;
			printf("[%#06d] %s %s %x\n", SYSTICK_counter,
				   "\033[36mINFO\033[0m",
				   "main: key_short=",
				   readkey);
		}

		readkey = get_key_short(GPIO_PIN_KEY2);
		if (readkey){
			output = CHARMENU_BUTTON_NEXT_DOWN;
			printf("[%#06d] %s %s %x\n", SYSTICK_counter,
				   "\033[36mINFO\033[0m",
				   "main: key_short=",
				   readkey);
		}

		readkey = get_key_short(GPIO_PIN_KEY3);
		if (readkey){
			output = CHARMENU_BUTTON_ENTER_DOWN;
			printf("[%#06d] %s %s %x\n", SYSTICK_counter,
				   "\033[36mINFO\033[0m",
				   "main: key_short=",
				   readkey);
		}

		readkey = get_key_long(GPIO_PIN_KEY3);
		if (readkey){
			output = CHARMENU_BUTTON_BACK_DOWN;
			printf("[%#06d] %s %s %x\n", SYSTICK_counter,
				   "\033[36mINFO\033[0m",
				   "main: key_long=",
				   readkey);
		}
	} while( output == 0);

	
	return (output);
}

uint8_t CharMenu__button_is_pressed(CharMenu *self, uint8_t button)
{
	return 0;
}

/**
 * print number to LCD current position,
 * @param number: the number to be printed
 * @param pad: the space reserved for the number.
 * 		for example:
 * 		 pad=4, number= 30
 * 			print "  30"
 * 		 pad=2, number= 30
 * 			print "30"
 * 		 pad=4, number= 2384
 * 			print "2384"
 * 		 pad=4, number= 9
 * 			print "   9"
 */
void CharMenu__lcd_number(CharMenu *self, uint8_t number, uint8_t pad)
{
}


/**
 * print scroll bar to the bottom part of LCD
 * @param pos: is the position of the scroll
 * @param scale: is the length of the scroll
 */
void CharMenu__print_scroll(CharMenu *self, uint8_t pos, uint8_t scale)
{

	return;
}





void Menu__destroy(Menu *self)
{
	free(self);
}


void Menu__add_child(Menu* self,Menu* child)
{
	/**
	 * add child to self as its child_tail
	 */
	child->parent = self;
	child->pos = self->children;
	if (self->child_tail==NULL)
	{
		//self has no any child yet
		self->child_head = child;
		self->child_tail = child;
		self->children = 1;
	}
	else
	{
		//append child to self's children
		self->child_tail->next = child;
		child->prev = self->child_tail;
		self->child_tail = child;
		self->children += 1;
	}
}

