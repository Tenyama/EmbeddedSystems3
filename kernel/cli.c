
#include "./mbox.h"
// #include "../uart/uart0.h"
#include "../game/background.h"
#include "../game/balls.h"
#include "../uart/uart1.h"
#include "draw.h"
#include "menu.h"
// #include "./video.h"
#define MAX_CMD_SIZE 100
#define HISTORY_SIZE 10
#define BACKSPACE 8
#define DELETE 127
#define COMMAND_SIZE 9
// Remember to chang COMMAND_SIZE when adding/removing commands
static char *command_list[] = {"help",     "showinfo", "baudrate",
                               "stopbits", "clear",    "display image",
                               "game",     "exit",     "play video"};
void cli() {
  static char history[HISTORY_SIZE][MAX_CMD_SIZE]; // Pre-allocate history
  static char cli_buffer[MAX_CMD_SIZE];
  static int index = 0;
  static int history_index = 0;
  static int current_index = 0;

  uart_puts("*.* Group1_OS *.* > ");

  while (1) {
    char c = uart_getc();

    // Handle backspace
    if (c == BACKSPACE || c == DELETE) {
      if (index > 0) {
        index--;
        uart_puts("\b \b");
        cli_buffer[index] = '\0';
      }
    } else if (c == '+') {
      if (current_index > 0) {
        current_index--;
        uart_puts("\n*.* Group1_OS *.* > ");
        string_copy(cli_buffer, history[current_index]);
        uart_puts(cli_buffer);
        index = string_length(cli_buffer);
      }
    } else if (c == '-') {
      if (current_index < history_index - 1) {
        current_index++;
        uart_puts("\n*.* Group1_OS *.* > ");
        string_copy(cli_buffer, history[current_index]);
        uart_puts(cli_buffer);
        index = string_length(cli_buffer);
      }
    } else if (c == '\t') { // handle match
      if (string_starts_with(cli_buffer, "help") && index >= 5) {
        cli_buffer[index] = '\0';
        char *match = "";
        int match_check = 0;

        for (int i = 0; i < COMMAND_SIZE; i++) {
          if (string_starts_with(command_list[i], cli_buffer + 5)) {
            match = command_list[i];
            match_check = 1;
          }
        }
        if (match_check) {
          string_copy(cli_buffer + 5, match);
          for (int i = 1; i < index - 5; i++) {
            uart_puts("\b \b");
          }
          uart_puts("\b \b");
          index = string_length(cli_buffer);
          uart_puts(match);
        }
      } else {
        cli_buffer[index] = '\0';

        char *match = "";
        int match_count = 0;

        for (int i = 0; i < COMMAND_SIZE; i++) {
          if (string_starts_with(command_list[i], cli_buffer)) {
            if (match_count == 0) {
              match = command_list[i];
            }
            match_count++;
          }
        }

        if (match_count == 1) {
          string_copy(cli_buffer, match);
          for (int i = 1; i < index; i++) {
            uart_puts("\b \b");
          }
          uart_puts("\b \b");
          index = string_length(cli_buffer);
          uart_puts(match);
        } else if (match_count > 1) {
          uart_puts("\nPossible matches:\n");
          for (int i = 0; i < COMMAND_SIZE; i++) {
            if (string_starts_with(command_list[i], cli_buffer)) {
              uart_puts(command_list[i]);
              uart_puts("\t");
            }
          }
          uart_puts("\n*.* Group1_OS *.* > ");
          uart_puts(cli_buffer);
        }
      }
    } else if (c == '\r' || c == '\n') { // Handle Enter key
      cli_buffer[index] = '\0';

      // copy cli_buffer into the current history entry manually
      for (int i = 0; i <= index; i++) {
        history[history_index][i] = cli_buffer[i]; // copy each character
      }
      uart_puts("\n");
      history_index = (history_index + 1) % HISTORY_SIZE;
      current_index = history_index;
      index = 0;
      break;
    } else if (index < MAX_CMD_SIZE - 1) {
      cli_buffer[index] = c;
      index++;
      uart_sendc(c); // Echothe character back
    }
  }
  if (string_compare(cli_buffer, "exit")) {
    uart_puts("Shutting down...\n");
    uart_puts("Press CTRL + C to return to Terminal\n");
    asm volatile("wfi");
  }
  if (string_starts_with(cli_buffer, "help ")) {
    char command[MAX_CMD_SIZE];
    string_copy(command, cli_buffer + 5); // Skip "help " and copy the rest
    handle_help_command(command);
  } else if (string_compare(cli_buffer, "help")) {
    draw_command_table(); // Show the command menu if just "help" is entered
  } else if (string_starts_with(cli_buffer,
                                "baudrate ")) { // Handle baudrate command
    handle_baudrate_command_uart1(cli_buffer);
  } else if (string_starts_with(cli_buffer,
                                "stopbits ")) { // Handle stopbits command
    handle_stopbits_command_uart1(cli_buffer);
  } else if (string_compare(cli_buffer, "showinfo")) {
    display_board_info(); // da ra output nhung chua check dung hay k
  } else if (string_compare(cli_buffer, "checkbaudrate")) {
    check_baudrate_uart1();
  } else if (string_compare(cli_buffer, "checkstopbits")) {
    check_stopbits_uart1();
  } else if (string_compare(cli_buffer, "display image")) {
    draw();
  } else if (string_compare(cli_buffer, "play video")) {
    // playVideo();
  } else if (string_compare(cli_buffer, "game")) {
    drawImage(0, 0, myBackground, 700, 800);
    initializeBalls();

    // drawBallsMatrix();
    copyBallsToScreen();

    eraseBall(257,29);
    testShootBall();

  } else if (string_compare(cli_buffer, "exit")) {
    uart_puts("Shutting down...\n");
    asm volatile("wfi");
  } else {
    uart_puts("Invalid command format. Use 'help' or 'help <command_name>' to "
              "get valid commands.\n");
  }
}
