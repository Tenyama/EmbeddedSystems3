// ----------------------------------- framebf.c -------------------------------------
#include "mbox.h"
#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include"../font/font.h"
// #include "videoFrame.h"
#define NULL ((void*)0)


//Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32

//Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0

// Screen info
unsigned int width, height, pitch;

/* Frame buffer address
 * (declare as pointer of unsigned char to access each byte) */

unsigned char *fb = NULL;


/**
 * Set screen resolution to 800x600
 */
void framebf_init()
{
    mBuf[0] = 35*4; // Length of message in bytes
    mBuf[1] = MBOX_REQUEST;

    mBuf[2] = MBOX_TAG_SETPHYWH; //Set physical width-height
    mBuf[3] = 8; // Value size in bytes
    mBuf[4] = 0; // REQUEST CODE = 0
    mBuf[5] = 700; // Value(width)
    mBuf[6] = 800; // Value(height)

    mBuf[7] = MBOX_TAG_SETVIRTWH; //Set virtual width-height
    mBuf[8] = 8;
    mBuf[9] = 0;
    mBuf[10] = 1400;
    mBuf[11] = 1600;

    mBuf[12] = MBOX_TAG_SETVIRTOFF; //Set virtual offset
    mBuf[13] = 8;
    mBuf[14] = 0;
    mBuf[15] = 0; // x offset
    mBuf[16] = 0; // y offset

    mBuf[17] = MBOX_TAG_SETDEPTH; //Set color depth
    mBuf[18] = 4;
    mBuf[19] = 0;
    mBuf[20] = COLOR_DEPTH; //Bits per pixel

    mBuf[21] = MBOX_TAG_SETPXLORDR; //Set pixel order
    mBuf[22] = 4;
    mBuf[23] = 0;
    mBuf[24] = PIXEL_ORDER;

    mBuf[25] = MBOX_TAG_GETFB; //Get frame buffer
    mBuf[26] = 8;
    mBuf[27] = 0;
    mBuf[28] = 16; //alignment in 16 bytes
    mBuf[29] = 0;  //will return Frame Buffer size in bytes

    mBuf[30] = MBOX_TAG_GETPITCH; //Get pitch
    mBuf[31] = 4;
    mBuf[32] = 0;
    mBuf[33] = 0; //Will get pitch value here

    mBuf[34] = MBOX_TAG_LAST;

    // Call Mailbox
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP) //mailbox call is successful ?
    	&& mBuf[20] == COLOR_DEPTH //got correct color depth ?
		&& mBuf[24] == PIXEL_ORDER //got correct pixel order ?
		&& mBuf[28] != 0 //got a valid address for frame buffer ?
		) {

    	/* Convert GPU address to ARM address (clear higher address bits)
    	 * Frame Buffer is located in RAM memory, which VideoCore MMU
    	 * maps it to bus address space starting at 0xC0000000.
    	 * Software accessing RAM directly use physical addresses
    	 * (based at 0x00000000)
    	*/
    	mBuf[28] &= 0x3FFFFFFF;

        // Access frame buffer as 1 byte per each address
        fb = (unsigned char *)((unsigned long)mBuf[28]);
        uart_puts("Got allocated Frame Buffer at RAM physical address: ");
        uart_hex(mBuf[28]);
        uart_puts("\n");

        uart_puts("Frame Buffer Size (bytes): ");
        uart_dec(mBuf[29]);
        uart_puts("\n");

        width = mBuf[5];     	// Actual physical width
        height = mBuf[6];     	// Actual physical height
        pitch = mBuf[33];       // Number of bytes per line

    } else {
    	uart_puts("Unable to get a frame buffer with provided setting\n");
    }
}


void drawPixelARGB32(int x, int y, unsigned int attr)
{
	int offs = (y * pitch) + (COLOR_DEPTH/8 * x);

/*	//Access and assign each byte
    *(fb + offs    ) = (attr >> 0 ) & 0xFF; //BLUE  (get the least significant byte)
    *(fb + offs + 1) = (attr >> 8 ) & 0xFF; //GREEN
    *(fb + offs + 2) = (attr >> 16) & 0xFF; //RED
    *(fb + offs + 3) = (attr >> 24) & 0xFF; //ALPHA
*/

	//Access 32-bit together
	*((unsigned int*)(fb + offs)) = attr;
}

unsigned int getPixelARGB32(int x, int y)
{
	int offs = (y * pitch) + (COLOR_DEPTH/8 * x);

/*	//Access and assign each byte
    *(fb + offs    ) = (attr >> 0 ) & 0xFF; //BLUE  (get the least significant byte)
    *(fb + offs + 1) = (attr >> 8 ) & 0xFF; //GREEN
    *(fb + offs + 2) = (attr >> 16) & 0xFF; //RED
    *(fb + offs + 3) = (attr >> 24) & 0xFF; //ALPHA
*/

	//Access 32-bit together
	unsigned int pixelColor = *((unsigned int*)(fb + offs));

    return pixelColor;
}


void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
	for (int y = y1; y <= y2; y++ )
		for (int x = x1; x <= x2; x++) {
			if ((x == x1 || x == x2) || (y == y1 || y == y2))
				drawPixelARGB32(x, y, attr);
			else if (fill)
				drawPixelARGB32(x, y, attr);
		}
}




// Function to draw line
void drawLine(int x1, int y1, int x2, int y2, unsigned int attr)
{
	for (int x = x1; x <= x2; x++ ){
        int y = (float)(y1 - y2)/(x1 - x2)*(x - x1) + y1;
        drawPixelARGB32(x, y, attr);
    }
}

void drawLineVetical(int x1, int y1, int x2, int y2, unsigned int attr) {
    // Calculate the differences in x and y
    int dx = (x2 >= x1) ? (x2 - x1) : (x1 - x2);  // Manual abs() for dx
    int dy = (y2 >= y1) ? (y2 - y1) : (y1 - y2);  // Manual abs() for dy

    // Determine the direction of the step (positive or negative)
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    // Error term
    int err = dx - dy;
    int e2;

    while (1) {
        // Draw the current pixel
        drawPixelARGB32(x1, y1, attr);

        // Check if we reached the end point
        if (x1 == x2 && y1 == y2) {
            break;
        }

        e2 = 2 * err;

        // Adjust the x-coordinate
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        // Adjust the y-coordinate
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}




// Function to calculate the square root of a number using the Newton-Raphson method
double sqrt(double number) {
    if (number < 0) {
        return -1; // Return -1 for negative inputs as square root of negative is not defined in real numbers
    }
    
    double tolerance = 0.000001; // Define the tolerance for the result
    double guess = number / 2.0; // Initial guess (can be any positive number, here half of the number)
    double result = 0.0;
    
    while (1) {
        result = 0.5 * (guess + number / guess); // Calculate the next approximation
        
        // Check if the difference between the current guess and the new result is within the tolerance
        int diff = (result > guess) ? (result - guess) : (guess - result);
        if (diff < tolerance) {
            break;
        }
        
        guess = result; // Update the guess for the next iteration
    }
    
    return result;
}


// Function to draw circle
void drawLCircle(int center_x, int center_y, int radius, unsigned int attr, int fill)
{
    //Draw the circle when going on x side
    for (int x = center_x - radius; x <= center_x + radius; x++) {
        // Calculate the corresponding y values using the circle equation
        int dy = sqrt(radius * radius - (x - center_x) * (x - center_x)); 
        int upper_y = center_y + dy;
        int lower_y = center_y - dy;

        drawPixelARGB32(x, upper_y, attr);
        drawPixelARGB32(x, lower_y, attr);

        // Fill the circle, draw a line between lower_y and upper_y
        if (fill) {
            for (int y = lower_y; y <= upper_y; y++) {
                drawPixelARGB32(x, y, attr);
            }
        }
    }

    /* Also draw the circle border when going on y side (
    since some points may be missing due to inaccurate calculation above) */

    for (int y = center_y - radius; y <= center_y + radius; y++) {
        // Calculate the corresponding x values using the circle equation
        int dx = sqrt(radius * radius - (y - center_y) * (y - center_y)); 

        int left_x = center_x - dx;
        int right_x = center_x + dx;

        drawPixelARGB32(left_x, y, attr);
        drawPixelARGB32(right_x, y, attr);
    }
}

void draw_char(int x, int y, char c, unsigned int color, int scale) {
  if (c < 32 || c > 126)
    return; // Ensure valid character range within 32 to 126

  int index = c - 32; // Adjust for the starting character (space ' ')

  for (int row = 0; row < FONT_HEIGHT; row++) {
    for (int col = 0; col < FONT_WIDTH; col++) {
      if (font_data[index][row] & (1 << (FONT_WIDTH - 1 - col))) {
        // Scaling the character
        for (int i = 0; i < scale; i++) {
          for (int j = 0; j < scale; j++) {
            drawPixelARGB32(x + col * scale + i, y + row * scale + j, color);
          }
        }
      }
    }
  }
}
void draw_string(int x, int y, const char *str, unsigned int color, int scale) {
  while (*str) {
    draw_char(x, y, *str++, color, scale);
    x += 8 * scale; // Move x for the next character
  }
}
// display video
void drawImageVideo(int start_x, int start_y, const unsigned int data[],
                    int imageWidth, int imageHeight) {
  int index = 0;
  for (int y = start_y; y < start_y + imageHeight; y++) {
    for (int x = start_x; x < start_x + imageWidth; x++) {
      if (data[index] != 0) { // Only draw if the pixel is not black
        drawPixelARGB32(x, y, data[index]);
      } else {                             // If the pixel is black, clear it
        drawPixelARGB32(x, y, 0x00000000); // Set to black
      }
      index++;
    }
  }
}
