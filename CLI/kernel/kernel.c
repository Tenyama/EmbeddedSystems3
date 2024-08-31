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
    }

    uart_puts("Embedded Systems 3 > ");

    // Reset the buffer index
    index = 0;
  }
}

void main() {
  // set up serial console
  uart_init();
  uart_puts("Hello World!\n");

  // Display the initial prompt
  uart_puts("Embedded Systems 3 > ");

  // run CLI
  while (1) {
    cli();
  }
}
