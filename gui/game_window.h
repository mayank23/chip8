#include <SDL.h>

#define SCALE 3
#define WINDOW_HEIGHT 32
#define WINDOW_WIDTH 64
#define TRUE 1
#define FALSE 0

extern SDL_Window *game_window;
extern SDL_Renderer* game_window_renderer;

extern unsigned char screen_pixels[WINDOW_HEIGHT][WINDOW_WIDTH];


typedef struct{
  unsigned char r,g,b,a;
} draw_color;

extern draw_color black_color;
extern draw_color white_color;


void setup_game_window(draw_color* initial_color);
void destroy_game_window();
void clear_window(draw_color* overlay_color);
void draw_pixel(unsigned int x,unsigned int y, draw_color* pixel_color);