// ----------------------------------- framebf.h -------------------------------------
void framebf_init();
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void drawLine(int x1, int y1, int x2, int y2, unsigned int attr);
void drawLineVetical(int x1, int y1, int x2, int y2, unsigned int attr);
void drawLCircle(int center_x, int center_y, int radius, unsigned int attr, int fill);
double sqrt(double number);
void drawImageVideo(int start_x, int start_y, const unsigned int data[], int imageWidth, int imageHeight);
void drawImage(int start_x, int start_y, const unsigned int data[], int imageWidth, int imageHeight);
unsigned int getPixelARGB32(int x, int y);