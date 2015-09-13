#include <SDL.h>

#define WINDOW_HEIGHT 64*10
#define WINDOW_WIDTH 32*10
#define TRUE 1
#define FALSE 0

extern SDL_Window *game_window;
extern SDL_Renderer* game_window_renderer;

extern unsigned char screen_pixels[WINDOW_HEIGHT][WINDOW_WIDTH];


typedef struct{
  unsigned char r,g,b,a;
} draw_color;

typedef SDL_Point Point;

extern Point points[WINDOW_HEIGHT][WINDOW_WIDTH];
void setup_game_window(draw_color* initial_color);
void destroy_game_window();
void clear_window(draw_color* overlay_color);
void draw_pixel(Point* point, draw_color* pixel_color);