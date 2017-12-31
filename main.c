//*****************************************************************************
// main.c
// Author: Brendan Smick, Kevin Mui, Jared Merkel, Micah DeRusha
//*****************************************************************************
#include "lab_buttons.h"
#include "BuckyBadger.black.h"

#define LOCAL_ID     0x86
#define REMOTE_ID    0x68

#define UP_BUTTON			0x00000001
#define DOWN_BUTTON 	0x00000002
#define LEFT_BUTTON 	0x00000004
#define RIGHT_BUTTON 	0x00000008
#define LOST 					0x00000016
#define BLOCKED 			0x00000032
#define HIT						0x00000064
#define JUNK					0x00000128

#define LCD_SIZE_X 240// X Size of LCD screen in pixels
#define LCD_SIZE_Y 320

	#define LCD_HALF_SIZE_X LCD_SIZE_X/2// X center of screen in pixels
	#define LCD_HALF_SIZE_Y LCD_SIZE_Y/2// Y center of screen in pixels


#define LCD_INIT_X LCD_HALF_SIZE_X - BUCKY_WIDTH_PXL/2// Start image in the center of the screen X
#define LCD_INIT_Y LCD_HALF_SIZE_Y - BUCKY_HEIGHT_PXL/2// Start image in the center of the screen Y



                                                              


/******************************************************************************
 * Global Variables
 *****************************************************************************/

	void reset(void);
	void draw_rect(uint16_t color, uint32_t rx_data);
	void attack(uint32_t buttons);

	char msg[80];
  uint32_t rx_data;
  uint32_t tx_data;
  uint8_t buttons;
  int wait_time = 200;
	int attack_delay = 6;
	int current_cycle = 1;
	int health = 8;
	bool getting_attacked = false;
	bool is_attacking = false;
	uint32_t our_attack_direction = 0;
	uint32_t defense_direction = 0;
	uint32_t attack_direction = 0;
	uint16_t curr_xDirection = 0;
	uint16_t curr_yDirection = 0;
//*****************************************************************************
//*****************************************************************************
int
main(void)
{
our_attack_direction = 0;
uint16_t width_pixels = BUCKY_WIDTH_PXL;// width of the image in pixels
uint16_t height_pixels = BUCKY_HEIGHT_PXL;// height of the image in pixels

	ece210_initialize_board();
	ece210_lcd_draw_image(LCD_INIT_X,width_pixels, LCD_INIT_Y,height_pixels, bucky_bitmap,LCD_COLOR_BLUE, LCD_COLOR_BLACK);
  //ece210_lcd_add_msg("Only Bug: First Attack Doesn't Work",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
 // ece210_lcd_add_msg("Second Attack must be by the other player.",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
  ece210_wireless_init(LOCAL_ID,REMOTE_ID);
	//ece210_wireless_send(JUNK);
	//int i = 0;
	for (int i =0; i < 8; i++) {
		ece210_ws2812b_write(i, 0, 20, 0);
	}
	while(1)
  {
	if (health <= 0){
					ece210_lcd_add_msg("Defeated", TERMINAL_ALIGN_CENTER, LCD_COLOR_RED);
					ece210_lcd_add_msg("Defeated", TERMINAL_ALIGN_CENTER, LCD_COLOR_RED);
					ece210_lcd_add_msg("Defeated", TERMINAL_ALIGN_CENTER, LCD_COLOR_RED);
					ece210_wireless_send(LOST);
					break;
				}
				
			// SENDS ATTACKS
    if(AlertButtons) {
      AlertButtons = false;
			//attack if ready
			if (!getting_attacked && current_cycle == 1) {
				our_attack_direction = ece210_buttons_read();
					ece210_wireless_send(our_attack_direction);
					is_attacking = true;
			}
    }
		//Receives attacks
    if(ece210_wireless_data_avaiable()) {
      rx_data = ece210_wireless_get();
			if( rx_data == LOST){
				ece210_lcd_add_msg("Victory", TERMINAL_ALIGN_CENTER, LCD_COLOR_RED);
				ece210_lcd_add_msg("Victory", TERMINAL_ALIGN_CENTER, LCD_COLOR_RED);
				ece210_lcd_add_msg("Victory", TERMINAL_ALIGN_CENTER, LCD_COLOR_RED);
				break;
			}
			else if (rx_data == BLOCKED) {
				health--;
				ece210_ws2812b_write(health, 20, 0, 0);
				getting_attacked = false;
			}
			else {
				if(!getting_attacked){
				// Draw Red Rectangle on Screen with Direction Attacked
				attack_direction = rx_data;
				draw_rect(LCD_COLOR_RED, attack_direction);
				getting_attacked = true;
				}
			}
    }
		curr_xDirection = ece210_ps2_read_x();
		 curr_yDirection = ece210_ps2_read_y();
		 if (curr_xDirection >= PS2_THRESHOLD_X_HI){
		 	defense_direction =  LEFT_BUTTON;
		 }
		 else if (curr_yDirection <= PS2_THRESHOLD_X_LO) {
		 	defense_direction = DOWN_BUTTON;
		 }
		 else if (curr_yDirection >= PS2_THRESHOLD_Y_HI) {
		 	defense_direction = UP_BUTTON;
		 }
		 else if (curr_xDirection <= PS2_THRESHOLD_Y_LO) {
		 	defense_direction = RIGHT_BUTTON;
		 }
		//Processes attack damages
		if(current_cycle == attack_delay){
			if( getting_attacked){
			if(is_attacking){
				if(attack_direction != our_attack_direction){
					health--;
					ece210_ws2812b_write(health, 20, 0, 0);
				}
			}
			else if(attack_direction != defense_direction){
				health--;
				ece210_ws2812b_write(health, 20, 0, 0);
		}
			else if(attack_direction == defense_direction){
				ece210_wireless_send(BLOCKED);
			}
		}
			current_cycle = 1;
			getting_attacked = false;
			is_attacking = false;
			our_attack_direction = 0;
			buttons = 0;
			draw_rect(LCD_COLOR_BLACK, attack_direction);
	
	}
		// Increment Cycle
		if(getting_attacked || is_attacking){
			current_cycle++;
		}
		ece210_wait_mSec(wait_time);
  }
}


void draw_rect(uint16_t color, uint32_t direction) {
	switch(direction) {
		case UP_BUTTON:
			ece210_lcd_draw_rectangle  (100 , 50, 0, 4, color);
			break;
		case DOWN_BUTTON:
			ece210_lcd_draw_rectangle  (100 , 50, 390, 4, color);
			break;
		case LEFT_BUTTON:
			ece210_lcd_draw_rectangle  (0 , 4, 150, 50, color);
			break;
		case RIGHT_BUTTON:
			ece210_lcd_draw_rectangle  (250, 4, 150, 50, color);
			break;
	}
}



