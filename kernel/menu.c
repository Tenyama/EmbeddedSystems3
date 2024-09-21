/*This file handles the commands format*/
#include "./menu.h"
#include "../uart/uart1.h"
#include "./mbox.h"
#include "framebf.h"

#define MAX_CMD_SIZE 100
#define BACKSPACE 8
#define DELETE 127
#define CLEAR_COLOR 0x00000000  // Black color in ARGB format

int stopbits = 1;

// /* Frame buffer address
//  * (declare as pointer of unsigned char to access each byte) */
// unsigned char *fb;

// method to compare two string
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

// this mt
int string_starts_with(const char *str, const char *prefix) {
  while (*prefix) {
    if (*str++ != *prefix++) {
      return 0;
    }
  }
  return 1;
}

void string_copy(char *dest, const char *src) {
  while (*src) {
    *dest++ = *src++;
  }
  *dest = '\0';
}

int string_length(char x[MAX_CMD_SIZE]) {
  int length = 0;
  while (x[length] != '\0') {
    length++;
  }
  return length;
}

void handle_help_command(char command[]) {
  if (string_compare(command, "showinfo")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "---------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "                   |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "-------------------+\n");
    uart_puts("|           | showinfo             | Show board revision and "
              "MAC address           |\n");
    uart_puts("|           |                      | Example: MyOS> showinfo    "
              "                   |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "-------------------+\n");
  } else if (string_compare(command, "clear")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "---------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "                   |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "-------------------+\n");
    uart_puts("|           | clear                | Clear QEMU screen          "
              "                   |\n");
    uart_puts("|           |                      | Example: MyOS> clear       "
              "                   |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "-------------------+\n");
    // You may want to implement a screen clearing function here.
  } else if (string_compare(command, "exit")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "---------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "                   |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "-------------------+\n");
    uart_puts("|           | exit                 | Exit the program                              |\n");
    uart_puts("|           |                      | Example: MyOS> clear       "
              "                   |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "-------------------+\n");
    // Exit logic here
  } else if (string_compare(command, "baudrate")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "--------------------------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "                                    |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------------------------+\n");
    uart_puts("|           | baudrate             | The baudrate can be set to any values                          |\n");
    uart_puts("|           |                      | Example: MyOS> baudrate "
              "                                       |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------------------------+\n");
  } else if (string_compare(command, "checkbaudrate")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "--------------------------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "                                    |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------------------------+\n");
    uart_puts("|           | checkbaudrate        | To check the baudrate values                                   |\n");
    uart_puts("|           |                      | Example: MyOS> checkbaudrate "
              "9600                              |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------------------------+\n");
  } else if (string_compare(command, "stopbits")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "--------------------------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "                                    |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------------------------+\n");
    uart_puts(
        "|           | stopbits             | The stopbits can be set to "
        "the values such as: 1, 2            |\n");
    uart_puts(
        "|           |                      | Example: MyOS> stopbits 1   "
        "                                   |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------------------------+\n");
    // Set stopbit logic here
  } else if (string_compare(command, "checkstopbits")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "----+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "  |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "--+\n");
    uart_puts(
        "|           | checkstopbits        | To check the stopbits  "
        "      |\n");
    uart_puts(
        "|           |                      | Example: MyOS> checkstopbits"
        " |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "--+\n");
    // Set stopbit logic here
  }else if (string_compare(command, "display image")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "--------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "                  |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------+\n");
    uart_puts("|           | display image        | Display team image                           |\n");
    uart_puts("|           |                      | Example: MyOS> display image"
              "                 |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------+\n");
    // Call a function to display the image here
  } else if (string_compare(command, "game")) {
    uart_puts("\n+-----------+----------------------+--------------------------"
              "--------------------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                      "
              "                              |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------------------+\n");
    uart_puts(
        "|           | game                 | To start playing Dynomite                                |\n");
    uart_puts(
        "|           |                      | Example: MyOS> game   "
        "                                   |\n");
    uart_puts("+-----------+----------------------+----------------------------"
              "------------------------------+\n");
    // Set stopbit logic here
  }else {
    uart_puts("Unknown command: ");
    uart_puts(command);
    uart_puts("\n");
  }
}

// setup for "showinfo" command
void display_board_info() {
  // Prepare mailbox buffer
  mBuf[0] = 8 * 4; // Length of the message in bytes
  mBuf[1] = MBOX_REQUEST;

  mBuf[2] = MBOX_TAG_GETBOARDREV; // Get board revision
  mBuf[3] = 4;                    // Size of the value buffer
  mBuf[4] = 0;                    // Request code
  mBuf[5] = 0;                    // Placeholder for board revision

  mBuf[6] = MBOX_TAG_GETMACADDR; // Get MAC address
  mBuf[7] = 6;                   // Size of the MAC address (6 bytes)
  mBuf[8] = 0;                   // Request code
  mBuf[9] = 0;                   // MAC address part 1 (most significant bytes)
  mBuf[10] = 0;                  // MAC address part 2 (least significant bytes)

  mBuf[11] = MBOX_TAG_LAST;

  // Make mailbox call
  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
    // Display board revision
    uart_puts("\nBoard Revision: ");
    uart_hex(mBuf[5]); // Display the board revision in hex format
    uart_puts("\n");

    // Display MAC address
    uart_puts("MAC Address: ");
    uart_hex(mBuf[9] >> 16); // First two bytes
    uart_puts(":");
    uart_hex((mBuf[9] >> 8) & 0xFF); // Second byte
    uart_puts(":");
    uart_hex(mBuf[9] & 0xFF); // Third byte
    uart_puts(":");
    uart_hex(mBuf[10] >> 16); // Fourth byte
    uart_puts(":");
    uart_hex((mBuf[10] >> 8) & 0xFF); // Fifth byte
    uart_puts(":");
    uart_hex(mBuf[10] & 0xFF); // Sixth byte
    uart_puts("\n");
  } else {
    uart_puts("Unable to retrieve board info and MAC address.\n");
  }
}

/**
 * Handle baudrate command (e.g., "baudrate 115200")
 */

int string_to_int(const char *str) {
  int result = 0;
  while (*str >= '0' && *str <= '9') {
    result = result * 10 + (*str - '0');
    str++;
  }
  return result;
}
// Helper function to convert an integer to a string (manually, without libraries)
void int_to_str(int value, char *str) {
    int i = 0;
    int is_negative = 0;

    // Handle negative numbers
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    // Process each digit and store it in the string
    do {
        str[i++] = (value % 10) + '0';  // Convert digit to character
        value /= 10;
    } while (value);

    // If the number was negative, add the '-' sign
    if (is_negative) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string since we stored the digits in reverse order
    int start = 0;
    int end = i - 1;
    char temp;
    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void handle_baudrate_command_uart1(char *command) {
  char *baudrate_value = command + 9; // Skip "baudrate "
  while (*baudrate_value == ' ')
    baudrate_value++;                       // Trim spaces
  baudrate = string_to_int(baudrate_value); // Convert string to int

  if (baudrate == 115200 || baudrate == 9600) {
    set_baudrate_uart1(baudrate);
  } else {
    uart_puts("Invalid baudrate. Use 'baudrate 9600' or 'baudrate 115200'.\n");
  }
}

/**
 * Handle stopbits command (e.g., "stopbits 1" or "stopbits 2")
 */
void handle_stopbits_command_uart1(char *command) {
  char *stopbit_value = command + 9; // Skip "stopbits "
  while (*stopbit_value == ' ')
    stopbit_value++; // Trim spaces

  if (string_compare(stopbit_value, "1")) {
    set_stopbits_uart1(1); // Set to 1 stop bit
  } else if (string_compare(stopbit_value, "2")) {
    set_stopbits_uart1(2); // Set to 2 stop bits (not supported by Mini UART)
  } else {
    uart_puts("Invalid stopbits value. Use 'stopbits 1'.\n");
  }
}

// Function to clear the entire screen
void clearScreen() {
    framebf_init();
    // Get the current screen resolution
    unsigned int width = mBuf[5];  // Screen width
    unsigned int height = mBuf[6];  // Screen height

    // Iterate over every pixel on the screen and set it to CLEAR_COLOR
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            drawPixelARGB32(x, y, CLEAR_COLOR);
        }
    }
}
void draw_command_table() {
    uart_puts("                    ==================================== \n");
    uart_puts("                    ||          COMMAND MENU          || \n");
    uart_puts("                    ==================================== \n");
    uart_puts("\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | Command # |     Command Name     |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 1         | help                 |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 2         | clear                |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 3         | showinfo             |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 4         | baudrate             |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 5         | checkbaudrate        |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 6         | stopbits             |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 7         | checkstopbits        |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 8         | display image        |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 9         | display video        |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 10        | game                 |\n");
    uart_puts("                    +-----------+----------------------+\n");
    uart_puts("                    | 11        | exit                 |\n");
    uart_puts("                    +-----------+----------------------+\n");
}

void print_logo() {
  // Printing the welcome ASCII art using uart_puts
  uart_puts("\n");
  uart_puts("      ::::::::::       ::::::::::       ::::::::::   :::::::::::  "
            "                         ::::::::         :::       ::::::::      "
            " ::::::: \n");
  uart_puts("     :+:              :+:              :+:              :+:       "
            "                       :+:    :+:       :+:       :+:    :+:     "
            ":+:   :+: \n");
  uart_puts("    +:+              +:+              +:+              +:+        "
            "                            +:+       +:+ +:+    +:+    +:+     "
            "+:+   +:+  \n");
  uart_puts("   +#++:++#         +#++:++#         +#++:++#         +#+         "
            "  +#++:++#++:++          +#+        +#+  +:+     +#++:++#+     "
            "+#+   +:+   \n");
  uart_puts("  +#+              +#+              +#+              +#+          "
            "                      +#+         +#+#+#+#+#+         +#+     +#+ "
            "  +#+    \n");
  uart_puts(" #+#              #+#              #+#              #+#           "
            "                    #+#                #+#    #+#    #+#     #+#  "
            " #+#     \n");
  uart_puts("##########       ##########       ##########       ###            "
            "                  ##########          ###     ########       "
            "#######       \n");
  uart_puts("                                                                  "
            "                                                                  "
            "         \n");
  uart_puts("                                                                  "
            "                                                                  "
            "         \n");
  uart_puts("                                                                  "
            "                                                                  "
            "         \n");
  uart_puts("                                                                  "
            "                                                                  "
            "         \n");
  uart_puts("                                                                  "
            "                                                                  "
            "         \n");
  uart_puts("                                                                  "
            "                                                                  "
            "         \n");
  uart_puts("                                                                  "
            "                                                                  "
            "         \n");
  uart_puts("      :::::::::           :::        :::::::::       ::::::::::   "
            "     ::::::::       ::::::::                                      "
            "         \n");
  uart_puts("     :+:    :+:        :+: :+:      :+:    :+:      :+:           "
            "   :+:    :+:     :+:    :+:                                      "
            "         \n");
  uart_puts("    +:+    +:+       +:+   +:+     +:+    +:+      +:+            "
            "  +:+    +:+     +:+                                              "
            "         \n");
  uart_puts("   +#++:++#+       +#++:++#++:    +#++:++#:       +#++:++#        "
            " +#+    +:+     +#++:++#++                                        "
            "         \n");
  uart_puts("  +#+    +#+      +#+     +#+    +#+    +#+      +#+              "
            "+#+    +#+            +#+                                         "
            "         \n");
  uart_puts(" #+#    #+#      #+#     #+#    #+#    #+#      #+#              "
            "#+#    #+#     #+#    #+#                                         "
            "          \n");
  uart_puts("#########       ###     ###    ###    ###      ##########        "
            "########       ########                                           "
            "          \n \n \n \n");

  uart_puts("=================================================================="
            "=========== \n");
  uart_puts("||                               TEAM MEMBERS                     "
            "         || \n");
  uart_puts("=================================================================="
            "=========== \n ");

  uart_puts(
      "          +---------------------------+----------------------------+\n");
  uart_puts("           |            SID            |          Full Name       "
            "  |\n");
  uart_puts("           "
            "+---------------------------+----------------------------+\n");

  uart_puts("           |         s3924729          |         HO QUANG HUY     "
            "  |\n");
  uart_puts("           |                           |                          "
            "  |\n");
  uart_puts("           "
            "+---------------------------+----------------------------+\n");

  uart_puts(
      "           |         s3986878          |      PHUNG THI MINH ANH    |\n");
  uart_puts("           |                           |                          "
            "  |\n");
  uart_puts("           "
            "+---------------------------+----------------------------+\n");

  uart_puts("           |         s3979297          |      TRINH PHUONG THAO   "
            "  |\n");
  uart_puts("           |                           |                          "
            "  |\n");
  uart_puts("           "
            "+---------------------------+----------------------------+\n");

  uart_puts("           |         s3977955          |        LE THIEN SON      "
            "  |\n");
  uart_puts("           |                           |                          "
  "  |\n");
  uart_puts(
      "           "
      "+---------------------------+----------------------------+\n\n\n\n");
}
