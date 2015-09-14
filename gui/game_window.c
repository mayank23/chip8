
#include "game_window.h"

void setup_game_window(draw_color* initial_color){
  SDL_CreateWindowAndRenderer(WINDOW_WIDTH*SCALE, WINDOW_HEIGHT*SCALE, 0, &game_window, &game_window_renderer);
  clear_window(initial_color);
}

void destroy_game_window(){
  draw_color color;
  color.r = 0;
  color.g = 0;
  color.b = 0;
  color.a = 255;
  clear_window(&color);
  SDL_DestroyWindow(game_window);
  SDL_Quit();
}

void clear_window(draw_color* overlay_color)
{
   SDL_SetRenderDrawColor(game_window_renderer, overlay_color->r, overlay_color->g, overlay_color->b, overlay_color->a);

   for(int i=0;i<WINDOW_HEIGHT;i++)
  {
    for(int j=0;j<WINDOW_WIDTH;j++)
    {
      screen_pixels[i][j] = 0;
      
    }
  }

  SDL_Rect pixel_rect;
  pixel_rect.x = 0;
  pixel_rect.y = 0;
  pixel_rect.w = WINDOW_WIDTH*SCALE;
  pixel_rect.h = WINDOW_HEIGHT*SCALE;


  SDL_RenderFillRect(game_window_renderer, &pixel_rect);
  SDL_RenderPresent(game_window_renderer);

}


void draw_pixel(unsigned int x, unsigned int y, draw_color* pixel_color)
{
  SDL_SetRenderDrawColor(game_window_renderer, pixel_color->r, pixel_color->g, pixel_color->b, pixel_color->a);
  SDL_Rect pixel_rect;
  pixel_rect.x = x;
  pixel_rect.y = y;
  pixel_rect.w = SCALE;
  pixel_rect.h = SCALE;


  SDL_RenderFillRect(game_window_renderer, &pixel_rect);
  SDL_RenderPresent(game_window_renderer);
}