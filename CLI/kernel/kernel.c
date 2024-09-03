// #include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "framebf.h"
#include "mbox.h"

#define MAX_CMD_SIZE 100
#define BACKSPACE 8
#define DELETE 127

int string_compare(const char *str1, const char *str2) {
  while (*str1 && *str2) {
    if (*str1 != *str2) {
      return 0;
    }
    str1++;
    str2++;
  }
  return (*str1 == '\0' && *str2 == '\0');
}

void cli() {
  static char cli_buffer[MAX_CMD_SIZE];
  static int index = 0;

  // read a character from UART
  char c = uart_getc();

  if (c == BACKSPACE || c == DELETE) {
    if (index > 0) {
      // Move the cursor back, print a space to clear the character, then move
      // the cursor back again
      index--;
      uart_sendc('\b');
      uart_sendc(' ');
      uart_sendc('\b');

      cli_buffer[index] = '\0';
    }
  } else if (c != '\n') {
    if (index < MAX_CMD_SIZE - 1) {
      cli_buffer[index] = c;
      index++;
      uart_sendc(c);
    }
  } else if (c == '\n') {
    cli_buffer[index] = '\0';

    uart_puts("\nGot commands: ");
    uart_puts(cli_buffer);
    uart_puts("\n");

    /* Compare with supported commands and execute*/
    if (string_compare(cli_buffer, "draw")) {
      framebf_init();
<<<<<<< HEAD
      }
    else if(string_compare(cli_buffer,"help"))
    {
      
    }
      uart_puts("*.* Group1_OS *.* > ");
=======
      drawRectARGB32(100, 100, 400, 400, 0x00AA0000, 1); // RED
      drawRectARGB32(150, 150, 400, 400, 0x0000BB00, 1); // GREEN
      drawRectARGB32(200, 200, 400, 400, 0x000000CC, 1); // BLUE
      drawRectARGB32(250, 250, 400, 400, 0x00FFFF00, 1); // YELLOW
      drawPixelARGB32(300, 300, 0x00FF0000);             // RED
    }
    if (string_compare(cli_buffer, "exit")) {
      uart_puts("Shutting down...\n");
      uart_puts("Press CTRL + C to return to Terminal\n");
      asm volatile("wfi");
    }

    uart_puts("Embedded Systems 3> ");

>>>>>>> 2b07278368b921113a69bcbee997e7259f42e456
    // Reset the buffer index
    index = 0;
    
    
  }
    

}

void main() {
  // set up serial console
  uart_init();

  //print out EEET2490 BARE OS 
  print_logo();

  
  draw_command_table();

  // Display the initial prompt
  uart_puts("*.* Group1_OS *.* > ");
  // run CLI
  while (1) {
    cli();
  }
}


