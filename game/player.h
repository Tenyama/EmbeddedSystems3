#ifndef PLAYER_SCOREBOARD_H
#define PLAYER_SCOREBOARD_H

// Player structure definition
typedef struct {
    int score;  // The player's current score
} Player;

// Function declarations
void initPlayer(Player *player);                // Initialize a player
void increaseScore(Player *player, int amount); // Increase the player's score
void decreaseScore(Player *player, int amount); // Decrease the player's score
void updatePlayerScoreDisplay(Player *player);  // Update the player's score display
void displayScoreRegion();                      // Display the scoreboard region

#endif // PLAYER_SCOREBOARD_H
