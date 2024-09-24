#include "uart1.h"
volatile unsigned int baudrate;

#define SYSTEM_CLOCK 250000000 // 250 MHz clock for Mini UART
/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init() {
  unsigned int r;
  baudrate = 270;

  /* initialize UART */
  AUX_ENABLE |= 1;   // enable mini UART (UART1)
  AUX_MU_CNTL = 0;   // stop transmitter and receiver
  AUX_MU_LCR = 3;    // 8-bit mode (also enable bit 1 to be used for RPI3)
  AUX_MU_MCR = 0;    // clear RTS (request to send)
  AUX_MU_IER = 0;    // disable interrupts8
  AUX_MU_IIR = 0xc6; // enable and clear FIFOs

  AUX_MU_BAUD = baudrate; // configure 115200 baud rate
                          // [system_clk_freq/(baud_rate*8) - 1]

  /* Note: refer to page 11 of ARM Peripherals guide for baudrate configuration
  (system_clk_freq is 250MHz by default) */

  /* map UART1 to GPIO pins 14 and 15 */
  r = GPFSEL1;
  r &= ~((7 << 12) | (7 << 15));      // clear bits 17-12 (FSEL15, FSEL14)
  r |= (0b010 << 12) | (0b010 << 15); // set value 0b010 (select ALT5:
                                      // TXD1/RXD1)
  GPFSEL1 = r;

  /* enable GPIO 14, 15 */
  // #ifdef RPI3  // RPI3
  GPPUD = 0; // No pull up/down control
  // Toogle clock to flush GPIO setup
  r = 150;
  while (r--) {
    asm volatile("nop");
  } // waiting 150 cycles
  GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
  r = 150;
  while (r--) {
    asm volatile("nop");
  } // waiting 150 cycles
  GPPUDCLK0 = 0; // flush GPIO setup

  // #else // RPI4
  r = GPIO_PUP_PDN_CNTRL_REG0;
  r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
  GPIO_PUP_PDN_CNTRL_REG0 = r;
  // #endif

  AUX_MU_CNTL = 3; // enable transmitter and receiver (Tx, Rx)
}

/**
 * Send a character
 */
void uart_sendc(char c) {
  // wait until transmitter is empty
  do {
    asm volatile("nop");
  } while (!(AUX_MU_LSR & 0x20));

  // write the character to the buffer
  AUX_MU_IO = c;
}

/**
 * Receive a character
 */
char uart_getc() {
  char c;

  // wait until data is ready (one symbol)
  do {
    asm volatile("nop");
  } while (!(AUX_MU_LSR & 0x01));

  // read it and return
  c = (unsigned char)(AUX_MU_IO);

  // convert carriage return to newline character
  return (c == '\r' ? '\n' : c);
}

char uart_getc_game() {
  char c;
  /* wait until data is ready (one symbol) */
  int i = 0;
  int total_time = 10000;
  do {
    asm volatile("nop");
    i++;
    if (i == total_time) {
      return '\0';
    }
  } while (!(AUX_MU_LSR & 0x01)); // - bit 1 FIFO hold at least 1 symbol
  /* read it and return */
  c = (char)(AUX_MU_IO); // - read from FIFO as char
  /* convert carriage return to newline */
  return (c == '\r'
              ? '\n'
              : c); // - check for carriage return if yes change it to Enter
}

/**
 * Display a string
 */
void uart_puts(char *s) {
  while (*s) {
    // convert newline to carriage return + newline
    if (*s == '\n')
      uart_sendc('\r');
    uart_sendc(*s++);
  }
}

/**
 * Display a value in hexadecimal format
 */
void uart_hex(unsigned int num) {
  uart_puts("0x");
  for (int pos = 28; pos >= 0; pos = pos - 4) {

    // Get highest 4-bit nibble
    char digit = (num >> pos) & 0xF;

    /* Convert to ASCII code */
    // 0-9 => '0'-'9', 10-15 => 'A'-'F'
    digit += (digit > 9) ? (-10 + 'A') : '0';
    uart_sendc(digit);
  }
}

/*
**
* Display a value in decimal format
*/
void uart_dec(int num) {
  // A string to store the digit characters
  char str[33] = {0};

  // Calculate the number of digits
  int len = 1;
  int temp = num;
  while (temp >= 10) {
    len++;
    temp = temp / 10;
  }

  // Store into the string and print out
  for (int i = 0; i < len; i++) {
    int digit = num % 10; // get last digit
    num = num / 10;       // remove last digit from the number
    str[len - (i + 1)] = digit + '0';
  }
  str[len] = '\0';

  uart_puts(str);
}

// Function to set the stopbits
void set_stopbits_uart1(int stopbits) {
  if (stopbits == 1) {
    uart_puts("Stopbits set to 1.\n");
    // UART1 only supports 1 stopbit
  } else {
    uart_puts("Only 1 stopbit is supported by this UART.\n");
  }
}

// Check baudrate
void check_baudrate_uart1() {
  unsigned int divisor = baudrate; // Read the current divisor
  uart_puts("Current baudrate divisor: ");
  uart_dec(divisor);
  uart_puts("\n");
}

// Directly set the divisor for baudrate
void set_baudrate_uart1(unsigned int user_baudrate_divisor) {
  // Disable UART temporarily to set baudrate
  AUX_MU_CNTL = 0; // Disable transmitter and receiver

  // Directly set the divisor based on user input
  AUX_MU_BAUD = user_baudrate_divisor;
  baudrate = user_baudrate_divisor; // Store globally

  // Re-enable UART transmitter and receiver
  AUX_MU_CNTL = 3;

  // Debugging: Print the divisor
  uart_puts("Set baudrate divisor: ");
  uart_dec(user_baudrate_divisor);
  uart_puts("\n");
}

void check_stopbits_uart1() {
  unsigned int lcr = AUX_MU_LCR; // Read the Line Control Register
  uart_puts("Current Line Control Register (AUX_MU_LCR): ");
  uart_hex(lcr);
  uart_puts("\n");

  // Check stopbits (this example assumes your hardware supports changing
  // stopbits)
  if (lcr & (1 << 2)) {
    uart_puts("2 stopbits are set.\n");
  } else {
    uart_puts("1 stopbit is set.\n");
  }
}
