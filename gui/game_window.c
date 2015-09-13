
#include "game_window.h"

void setup_game_window(draw_color* initial_color){
  SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &game_window, &game_window_renderer);
  clear_window(initial_color);
  // initialize the screen buffer.
  for(int i=0;i<WINDOW_HEIGHT;i++)
  {
    for(int j=0;j<WINDOW_WIDTH;j++)
    {
      screen_pixels[i][j] = 0;
      points[i][j].x = j;
      points[i][j].y = i;
    }
  }
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
      points[i][j].x = j;
      points[i][j].y = i;
      SDL_RenderDrawPoints(game_window_renderer,&points[i][j],1);
    }
  }
  SDL_RenderPresent(game_window_renderer);
}

void draw_pixels(Point* points, int n, draw_color* pixel_color){
  SDL_SetRenderDrawColor(game_window_renderer, pixel_color->r, pixel_color->g, pixel_color->b, pixel_color->a);
  SDL_RenderDrawPoints(game_window_renderer, (SDL_Point*)points, n);
  SDL_RenderPresent(game_window_renderer);

}

void draw_pixel(Point* point, draw_color* pixel_color)
{
  SDL_SetRenderDrawColor(game_window_renderer, pixel_color->r, pixel_color->g, pixel_color->b, pixel_color->a);
  SDL_Rect pixel_rect;
  pixel_rect.x = point->x *10;
  pixel_rect.y = point->y *10;
  pixel_rect.w = 10;
  pixel_rect.h = 10;


  SDL_RenderFillRect(game_window_renderer, &pixel_rect);
  SDL_RenderPresent(game_window_renderer);
}