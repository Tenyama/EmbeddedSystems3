#include "../kernel/framebf.h"
#include "../kernel/mbox.h"

#define SCORE_REGION_WIDTH 256
#define SCOREBOARD_BORDER_COLOR 0xFFEA7F7D   // Black background for the score region (opaque)
#define SCOREBOARD_BACKGROUND_COLOR 0xFF999999
// Function to overlay the scoreboard on the left-ha`d side
void displayScoreRegion() {
    // Draw a black rectangle in the grey space (SCORE_REGION_WIDTH wide)
    framebf_init();
    int height = mBuf[6]; // Actual physical height
    drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1, SCOREBOARD_BACKGROUND_COLOR , 1);
    drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1, SCOREBOARD_BORDER_COLOR , 0);
    draw_string(20, 50, "PLAYER SCORE:" ,0xFFFFFFFF, 2);
//     draw_string();


}

