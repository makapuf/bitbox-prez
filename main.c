// main.c

#include <stdint.h>
#include <bitbox.h>
#include "fatfs/ff.h"

#include "blitter.h"

#define PREZ_FILENAME "slides.btc"

#define SLIDE_W 640
#define SLIDE_H 480
#define NB_SLIDES 72

#define NB_BANKS 2

#define SLIDE_HEADER 520 // 4+4+512
#define SLIDE_DATA SLIDE_W*SLIDE_H/4
#define BANK_SIZE SLIDE_DATA/NB_BANKS

object *btc, *rect;
FATFS fatfs;
FIL prez_file;
FRESULT result;

int bank_id, slide_id;

char buffer [ SLIDE_HEADER+SLIDE_DATA ];

void game_init() 
{
    blitter_init(); // to be removed as engine shall be handled  automatically 

	result = f_mount(&fatfs,"",1); // mount now
	if (result==FR_OK) 
		result = f_open (&prez_file,PREZ_FILENAME,FA_READ); // open file	
	else 
		die(2,result);

	if (result==FR_OK) 
		message("ok, mounted & file opened ok\n");	
	else 
		die(3,result);

	btc=btc4_new(((uint32_t *)buffer),0,0,0);
	btc->w = SLIDE_W;
	btc->h = SLIDE_H;
	bank_id=10; // dont start now.
}

void handle_pad() 
{
	
	kbd_emulate_gamepad();

	static uint16_t old_buttons;
	uint16_t pressed_buttons = gamepad_buttons[0] & ~old_buttons;
	old_buttons = gamepad_buttons[0];

	if (pressed_buttons & (gamepad_right | gamepad_R | gamepad_A | gamepad_X ))
		slide_id +=1;
	if (pressed_buttons & (gamepad_left | gamepad_L))
		slide_id -=1;
	if (pressed_buttons & gamepad_up)
		slide_id -=10;
	if (pressed_buttons & gamepad_down)
		slide_id +=10;

	if (pressed_buttons) 
		bank_id=0;

	if (vga_frame == 200) 
		bank_id = 0; // after a while, refresh
	

	if (slide_id>=NB_SLIDES) 
		slide_id = NB_SLIDES-1;
	if (slide_id<0)
		slide_id=0;
}

void game_frame (void) {
	/*
	check button presse (handle events ?)
	L,gauche : precedent (si >0)
	R,droite, buttons (any) : suivant (si <nb)
	dn : +10(si <nb sinon nb)
	up : -10 (si >10 sinon 0)

	si any bouton_presse : bank_id=0;
	start : autoplay

	*/
	handle_pad();

	size_t bytes_read;

	// load bank to vram
	if (bank_id < NB_BANKS)
	{
		if (bank_id==0) {
			f_lseek(&prez_file, slide_id*(SLIDE_DATA+SLIDE_HEADER) ); 
			f_read (&prez_file, buffer, SLIDE_HEADER, &bytes_read);		
		} 
		f_read(&prez_file, &buffer[SLIDE_HEADER + BANK_SIZE*bank_id], BANK_SIZE, &bytes_read);
		bank_id++;
	}

}

