// #include "../uart/uart0.h"
#include "../kernel/draw.h"
#include "../font/font.c"
#include "../uart/uart1.h"
#include "./framebf.h"
#include "./image.h"
#include "./mbox.h"
#include "menu.h"

int set_virtual_offset(unsigned int x_offset, unsigned int y_offset) {
  mBuf[0] =
      8 *
      4; // Message Buffer Size in bytes (17 elements * 4 bytes (32 bit) each)
  mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)

  mBuf[2] = MBOX_TAG_SETVIRTOFF; // Set virtual offset
  mBuf[3] = 8;
  mBuf[4] = 0;
  mBuf[5] = x_offset; // x offset
  mBuf[6] = y_offset; // y offset

  mBuf[7] = MBOX_TAG_LAST;

  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP) && mBuf[5] == x_offset &&
      mBuf[6] == y_offset) {
    uart_puts(" Set virtual offset successfully!\n");
    return 1;
  } else {
    uart_puts(" Unable to set virtual offset!\n");
    return 0;
  }
}
void drawImage(int start_x, int start_y, const unsigned int data[], int width,
               int height) {
  int index = 0;
  for (int y = start_y; y < start_y + height; y++) {
    for (int x = start_x; x < start_x + width; x++) {
      if (data[index] != 0) { // only draw if the pixel is not black
        drawPixelARGB32(x, y, data[index]);
      }

      index++;
    }
  }
}
void draw() {

  drawImage(0, 0, myImage, 711, 654);
  // draw_string(222, 540, "Thao Trinh", 0xFFFF00, 4);  // Draw text at position
  // (10,10) with a yellow color
  draw_string(195, 550, "Thao Trinh", 0xFF0000FF, 2);
  draw_string(40, 550, "Huy Ho", 0xFFFF00, 2);
  draw_string(580, 550, "Son Le", 0x0000FF00, 2);
  draw_string(400, 550, "Anh Phung", 0xFFFF0000, 2);
  draw_string(20, 590, "s3924729", 0xFFFF00, 2);
  draw_string(210, 590, "s3979297", 0xFF0000FF, 2);
  draw_string(402, 590, "s3986878", 0xFFFF0000, 2);
  draw_string(570, 590, "s3977955", 0x0000FF00, 2);

  draw_string(300, 30, "EEET2490 ", 0xFF000000, 2);
  draw_string(55, 70, "Embedded System: OS and Interfacing", 0xFF000000, 2);

  /* Question b */
  unsigned int x_offset = 0;
  unsigned int y_offset = 0;

  // echo everything back
  while (1) {
    // read each char
    char c = uart_getc();

    if (c == 'a') { // move left
      x_offset += 20;
    } else if (c == 'd') { // move right
      if (x_offset >= 20)
        x_offset -= 20;
    } else if (c == 'w') { // move up
      y_offset += 20;
    } else if (c == 's') { // move down
      if (y_offset >= 20)
        y_offset -= 20;
    } else if (c == 'q') {
      break;
    }

    set_virtual_offset(x_offset, y_offset);

    // adding some delay
    int r = 1000;
    while (r--) {
      asm volatile("nop");
    }

    // send back
    uart_sendc(c);
  }
}
