
#include "game_window.h"

void setup_game_window(draw_color* initial_color){

  SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &game_window, &game_window_renderer);
  clear_window(initial_color);
}

void destroy_game_window(){
  SDL_DestroyWindow(game_window);
  SDL_Quit();
}

void clear_window(draw_color* overlay_color)
{
  SDL_SetRenderDrawColor(game_window_renderer, overlay_color->r, overlay_color->g, overlay_color->b, overlay_color->a);
  SDL_RenderClear(game_window_renderer);
  SDL_RenderPresent(game_window_renderer);
}

void draw_pixels(Point* points, int n, draw_color* pixel_color){
  SDL_SetRenderDrawColor(game_window_renderer, pixel_color->r, pixel_color->g, pixel_color->b, pixel_color->a);
  SDL_RenderDrawPoints(game_window_renderer, (SDL_Point*)points, n);
  SDL_RenderPresent(game_window_renderer);

}