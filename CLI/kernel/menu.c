                                                            /*This file handles the commands format*/
#include "../uart/uart1.h"
#include "framebf.h"
#include "mbox.h"
// #include "framebf.c"

#define MAX_CMD_SIZE 100
#define BACKSPACE 8
#define DELETE 127

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
int string_starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str++ != *prefix++) {
            return 0;
        }
    }
    return 1;
}

void string_copy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void handle_help_command(char command[]) {
    if (string_compare(command, "showinfo")) {
      uart_puts("\n+-----------+----------------------+-----------------------------------------------+\n");
      uart_puts("| Command # | Command Name         | Usage                                         |\n");
      uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
      uart_puts("|           | showinfo             | Show board revision and MAC address           |\n");
      uart_puts("|           |                      | Example: MyOS> showinfo                       |\n");
      uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
    } else if (string_compare(command, "clear")) {
        uart_puts("\n+-----------+----------------------+-----------------------------------------------+\n");
        uart_puts("| Command # | Command Name         | Usage                                         |\n");
        uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
        uart_puts("|           | clear                | Clear screen (scroll down to current cursor)  |\n");
        uart_puts("|           |                      | Example: MyOS> clear                          |\n");
        uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
        // You may want to implement a screen clearing function here.
    } else if (string_compare(command, "exit")) {
        uart_puts("\n+-----------+----------------------+-----------------------------------------------+\n");
        uart_puts("| Command # | Command Name         | Usage                                         |\n");
        uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
        uart_puts("|           | clear                | Clear screen (scroll down to current cursor)  |\n");
        uart_puts("|           |                      | Example: MyOS> clear                          |\n");
        uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
        // Exit logic here
    } else if (string_compare(command, "baudrate")) {
        uart_puts("\n+-----------+----------------------+--------------------------------------------------------------+\n");
        uart_puts("| Command # | Command Name         | Usage                                                          |\n");
        uart_puts("+-----------+----------------------+----------------------------------------------------------------+\n");
        uart_puts("|           | baudrate             | The baudrate can be set to the values such as: 9600, 115200,   |\n");
        uart_puts("|           |                      | Example: MyOS> baudrate 9600                                   |\n");
        uart_puts("+-----------+----------------------+----------------------------------------------------------------+\n");
    } else if (string_compare(command, "stopbit")) {
        uart_puts("\n+-----------+----------------------+--------------------------------------------------------------+\n");
        uart_puts("| Command # | Command Name         | Usage                                                          |\n");
        uart_puts("+-----------+----------------------+----------------------------------------------------------------+\n");
        uart_puts("|           | stopbit              | The stopbit can be set to the values such as: 1, 2             |\n");
        uart_puts("|           |                      | Example: MyOS> stopbit 1                                       |\n");
        uart_puts("+-----------+----------------------+----------------------------------------------------------------+\n");
        // Set stopbit logic here
    } else if (string_compare(command, "display image")) {
        uart_puts("\n+-----------+----------------------+--------------------------------------------------------------+\n");
        uart_puts("| Command # | Command Name         | Usage                                                          |\n");
        uart_puts("+-----------+----------------------+----------------------------------------------------------------+\n");
        uart_puts("|           | baudrate             | The baudrate can be set to the values such as: 9600, 115200,   |\n");
        uart_puts("|           |                      | Example: MyOS> baudrate 9600                                   |\n");
        uart_puts("+-----------+----------------------+----------------------------------------------------------------+\n");
        // Call a function to display the image here
    } else if (string_compare(command, "play game")) {
        uart_puts("Starting game...\n");
        // Call a function to start the game
    } else {
        uart_puts("Unknown command: ");
        uart_puts(command);
        uart_puts("\n");
    }
}

//setup for "showinfo" command
void display_board_info() {
    // Prepare mailbox buffer
    mBuf[0] = 8 * 4;      // Length of the message in bytes
    mBuf[1] = MBOX_REQUEST;

    mBuf[2] = MBOX_TAG_GETBOARDREV;  // Get board revision
    mBuf[3] = 4;  // Size of the value buffer
    mBuf[4] = 0;  // Request code
    mBuf[5] = 0;  // Placeholder for board revision

    mBuf[6] = MBOX_TAG_GETMACADDR;   // Get MAC address
    mBuf[7] = 6;  // Size of the MAC address (6 bytes)
    mBuf[8] = 0;  // Request code
    mBuf[9] = 0;  // MAC address part 1 (most significant bytes)
    mBuf[10] = 0; // MAC address part 2 (least significant bytes)

    mBuf[11] = MBOX_TAG_LAST;

    // Make mailbox call
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {
        // Display board revision
        uart_puts("\nBoard Revision: ");
        uart_hex(mBuf[5]);  // Display the board revision in hex format
        uart_puts("\n");

        // Display MAC address
        uart_puts("MAC Address: ");
        uart_hex(mBuf[9] >> 16);  // First two bytes
        uart_puts(":");
        uart_hex((mBuf[9] >> 8) & 0xFF);  // Second byte
        uart_puts(":");
        uart_hex(mBuf[9] & 0xFF);  // Third byte
        uart_puts(":");
        uart_hex(mBuf[10] >> 16);  // Fourth byte
        uart_puts(":");
        uart_hex((mBuf[10] >> 8) & 0xFF);  // Fifth byte
        uart_puts(":");
        uart_hex(mBuf[10] & 0xFF);  // Sixth byte
        uart_puts("\n");
    } else {
        uart_puts("Unable to retrieve board info and MAC address.\n");
    }
}

//setup for editing stopbits for "stopbits " command
void handle_stopbits_command(char *command) {
    // Skip the "stopbits " part, so command should now be either "1" or "2"
    char *stopbit_value = command + 9;  // "stopbits " is 9 characters long

    if (string_compare(stopbit_value, "1")) {
        stopbits = 1;  // Set stop bits to 1
        uart_puts("Stop bit successfully set to 1.\n");
    } else if (string_compare(stopbit_value, "2")) {
        stopbits = 2;  // Set stop bits to 2
        uart_puts("Stop bit successfully set to 2.\n");
    } else {
        uart_puts("Invalid stop bit value. Please use 'stopbits 1' or 'stopbits 2'.\n");
    }
}

void draw_command_table() {
  
    //EDITTING FOR MORE READABLE OUTPUT
    uart_puts("\n");
    uart_puts("\n");
    uart_puts("\n");
    uart_puts("\n");

    uart_puts("======================================================================================== \n");
    uart_puts("||                                      COMMAND MENU                                  || \n");
    uart_puts("======================================================================================== \n ");

    uart_puts("\n+-----------+----------------------+-----------------------------------------------+\n");
    uart_puts("| Command # | Command Name         | Usage                                         |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
    
    uart_puts("| 1         | help                 | Show brief information of all commands        |\n");
    uart_puts("|           |                      | Example: MyOS> help                           |\n");
    uart_puts("|           | help <command_name>  | Show full information of a specific command   |\n");
    uart_puts("|           |                      | Example: MyOS> help showinfo                  |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
    
    uart_puts("| 2         | clear                | Clear screen (scroll down to current cursor)  |\n");
    uart_puts("|           |                      | Example: MyOS> clear                          |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");

    uart_puts("| 3         | showinfo             | Show board revision and MAC address           |\n");
    uart_puts("|           |                      | Example: MyOS> showinfo                       |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");

    uart_puts("| 4         | baudrate             | Change UART baudrate                          |\n");
    uart_puts("|           |                      | Example: MyOS> baudrate 9600                  |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");

    uart_puts("| 5         | stopbit              | Change UART stopbit setting to 1 or 2         |\n");
    uart_puts("|           |                      | Example: MyOS> stopbit 1                      |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");

    uart_puts("| 6         | display image        | Display the image of our team                 |\n");
    uart_puts("|           |                      | Example: MyOS> display image                  |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");

    uart_puts("| 7         | play game            | Start game and display in QEMU                |\n");
    uart_puts("|           |                      | Example: MyOS> play game                      |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");

    uart_puts("| 8         | exit                 | exit                                          |\n");
    uart_puts("|           |                      | Example: MyOS> exit                           |\n");
    uart_puts("+-----------+----------------------+-----------------------------------------------+\n");
}

void print_logo() {
    // Printing the welcome ASCII art using uart_puts
    uart_puts("\n");
    uart_puts("      ::::::::::       ::::::::::       ::::::::::   :::::::::::                           ::::::::         :::       ::::::::       ::::::: \n");
    uart_puts("     :+:              :+:              :+:              :+:                              :+:    :+:       :+:       :+:    :+:     :+:   :+: \n");
    uart_puts("    +:+              +:+              +:+              +:+                                    +:+       +:+ +:+    +:+    +:+     +:+   +:+  \n");
    uart_puts("   +#++:++#         +#++:++#         +#++:++#         +#+           +#++:++#++:++          +#+        +#+  +:+     +#++:++#+     +#+   +:+   \n");
    uart_puts("  +#+              +#+              +#+              +#+                                +#+         +#+#+#+#+#+         +#+     +#+   +#+    \n");
    uart_puts(" #+#              #+#              #+#              #+#                               #+#                #+#    #+#    #+#     #+#   #+#     \n");
    uart_puts("##########       ##########       ##########       ###                              ##########          ###     ########       #######       \n");
    uart_puts("                                                                                                                                             \n");
    uart_puts("                                                                                                                                             \n");
    uart_puts("                                                                                                                                             \n");
    uart_puts("                                                                                                                                             \n");
    uart_puts("                                                                                                                                             \n");
    uart_puts("                                                                                                                                             \n");
    uart_puts("                                                                                                                                             \n");
    uart_puts("      :::::::::           :::        :::::::::       ::::::::::        ::::::::       ::::::::                                               \n");
    uart_puts("     :+:    :+:        :+: :+:      :+:    :+:      :+:              :+:    :+:     :+:    :+:                                               \n");
    uart_puts("    +:+    +:+       +:+   +:+     +:+    +:+      +:+              +:+    +:+     +:+                                                       \n");
    uart_puts("   +#++:++#+       +#++:++#++:    +#++:++#:       +#++:++#         +#+    +:+     +#++:++#++                                                 \n");
    uart_puts("  +#+    +#+      +#+     +#+    +#+    +#+      +#+              +#+    +#+            +#+                                                  \n");
    uart_puts(" #+#    #+#      #+#     #+#    #+#    #+#      #+#              #+#    #+#     #+#    #+#                                                   \n");
    uart_puts("#########       ###     ###    ###    ###      ##########        ########       ########                                                     \n \n \n \n");

    uart_puts("============================================================================= \n");
    uart_puts("||                               TEAM MEMBERS                              || \n");
    uart_puts("============================================================================= \n ");


    uart_puts("          +---------------------------+----------------------------+\n");
    uart_puts("           |            SID            |          Full Name         |\n");
    uart_puts("           +---------------------------+----------------------------+\n");
    
    uart_puts("           |         s3924729          |         HO QUANG HUY       |\n");
    uart_puts("           |                           |                            |\n");
    uart_puts("           +---------------------------+----------------------------+\n");
    
    uart_puts("           |         s3986878          |      PHUNG THI MINH ANH   |\n");
    uart_puts("           |                           |                            |\n");
    uart_puts("           +---------------------------+----------------------------+\n");

    uart_puts("           |         s3979297          |      TRINH PHUONG THAO     |\n");
    uart_puts("           |                           |                            |\n");
    uart_puts("           +---------------------------+----------------------------+\n");

    uart_puts("           |         s3977955          |        LE THIEN SON        |\n");
    uart_puts("           +---------------------------+----------------------------+\n\n\n\n");
}
