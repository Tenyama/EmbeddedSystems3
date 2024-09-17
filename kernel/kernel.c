#include "mbox.h"
// #include "video.h"
#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "framebf.h"
#include "displayGameFrame.h"
#include "game.h"
#include "../game/background.h"

#define NULL ((void*)0)
	extern unsigned char *fb;  // Declaration to use fb from framebf.c


void main()
{
    // set up serial console
	uart_init();

	// say hello
	uart_puts("\n\nHello World!\n");

	// Initialize frame buffer
	framebf_init();
	// playVideo();
	// if (fb != NULL) {
    //     uart_puts("\nFramebuffer initialized successfully.\n");
    // } else {
    //     uart_puts("\nFramebuffer initialization failed.\n");
    //     return;
    // }
	// displayGameIntro((600-512)/2, (800-512)/2);
	//gameLevelDisplay((600-512)/2, (800-700)/2);

	//Test for shooter
	// drawRectARGB32(0, 0, 600, 800, 0x11223344, 1);
	drawImage(0, 0, myBackground, 700, 800);
	moveShooter();
	// drawLineVetical(600/2, 800, 600/2, 800/2, 0xAABBCCDD);

	

	// echo everything back
	while(1) {
		//read each char
		char c = uart_getc();

		//send back 
		uart_sendc(c);
	}
}