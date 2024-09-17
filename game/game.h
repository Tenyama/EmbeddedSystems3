typedef struct
{
    int angle;
} Shooter;

typedef enum 
{
    LEFT,
    RIGHT
} Direction;

// Define a structure to store pixel information (ARGB format)
typedef struct {
    int x, y;
    unsigned int color;
} PixelData;

void move_left();
void move_right();
void moveShooter();
void drawShooter(int base_x, int base_y, int shooter_angle);
int get_sine(int angle);
int get_cosine(int angle);
unsigned int getBackgroundPixelColor(int x, int y);
void drawBackgroundPixel(int x, int y);
void eraseShooter(int base_x, int base_y, int shooter_angle);
void calculateShooterEndpoint(int base_x, int base_y, int shooter_angle, int *end_x, int *end_y);
void bounceShooter();
