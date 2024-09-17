// player.h

#ifndef PLAYER_H
#define PLAYER_H

// Define the Player structure
typedef struct {
    int score;  // The player's current score
} Player;

// Function declarations
void initPlayer(Player *player);               // Initialize the player
void increaseScore(Player *player, int amount); // Increase the player's score
void decreaseScore(Player *player, int amount); // Decrease the player's score
void updatePlayerScoreDisplay(Player *player);  // Update the player's score display

#endif

