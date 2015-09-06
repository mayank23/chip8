#include <SDL.h>

#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH 160

SDL_Window *game_window;
SDL_Renderer* game_window_renderer;

unsigned char screen_pixels[WINDOW_HEIGHT][WINDOW_WIDTH];

typedef struct{
  unsigned char r,g,b,a;
} draw_color;

typedef SDL_Point Point;

void setup_game_window(draw_color* initial_color);
void destroy_game_window();
void clear_window(draw_color* overlay_color);
void draw_pixels(Point* points, int n, draw_color* pixel_color);