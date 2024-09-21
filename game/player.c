// player_scoreboard.c

#include "../kernel/draw.h"    // For drawing functions
#include "../kernel/framebf.h" // For framebuffer-related functions
#include "../kernel/mbox.h"    // For mailbox handling
#include "../kernel/menu.h"

#define SCORE_REGION_WIDTH 228
#define SCOREBOARD_BORDER_COLOR 0xFFEA7F7D // Border color for the scoreboard
#define SCOREBOARD_BACKGROUND_COLOR                                            \
  0xFF999999 // Background color for the scoreboard

// Define the Player structure
typedef struct {
  int score; // The player's current score
} Player;

// Function to initialize a player with a starting score
void initPlayer(Player *player) {
  player->score = 0; // Start the player with a score of 0
}

// Function to update the player's score display
void updatePlayerScoreDisplay(Player *player) {
  // Initialize the frame buffer (if not already done)
  framebf_init();

  int height = mBuf[6]; // Get the actual height of the screen

  // Clear the entire scoreboard region on the left side by redrawing the
  // background
  drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1,
                 SCOREBOARD_BACKGROUND_COLOR, 1);
  drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1,
                 SCOREBOARD_BORDER_COLOR, 0);

  // Coordinates for text
  int x = 20;
  int y = 50;
  unsigned int text_color = 0xFFFFFFFF; // White color for the text
  int scale = 2;                        // Text size

  // Redraw "PLAYER SCORE:" to refresh the background and text
  draw_string(x, y, "PLAYER SCORE:", text_color, scale);

  // Convert the player's score to a string
  char score_str[12];
  int_to_str(player->score, score_str); // Manually convert score to string

  // Display the player's score below the "PLAYER SCORE:" text
  draw_string(x, y + 40, score_str, text_color, scale);
}
// Function to increase the player's score by a certain amount
void increaseScore(Player *player, int amount) {
  player->score += amount;          // Add the amount to the player's score
  updatePlayerScoreDisplay(player); // Update the display after score changes
}

// Function to decrease the player's score by a certain amount
void decreaseScore(Player *player, int amount) {
  if (player->score >= amount) {
    player->score -= amount; // Subtract the amount from the player's score
  } else {
    player->score = 0; // Ensure the score doesn't go below 0
  }
  updatePlayerScoreDisplay(player); // Update the display after score changes
}

// Function to display the scoreboard region on the left-hand side
void displayScoreRegion() {
  // Initialize the frame buffer
  framebf_init();

  int height = mBuf[6]; // Get the actual height of the screen

  // Draw the background and border for the scoreboard region
  drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1,
                 SCOREBOARD_BACKGROUND_COLOR, 1);
  drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1,
                 SCOREBOARD_BORDER_COLOR, 0);

  // Display the "PLAYER SCORE:" label
  draw_string(20, 50, "PLAYER SCORE:", 0xFFFFFFFF, 2);
}
