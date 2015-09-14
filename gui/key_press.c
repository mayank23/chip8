#include <SDL.h>
#include "key_press.h"
#include "game_window.h"

int get_key_press(){

  SDL_Event e;
  if(SDL_PollEvent(&e)!=0)
  {
    if(e.type == SDL_KEYDOWN)
    {
      switch(e.key.keysym.sym)
      {
        case SDLK_1:{
          key_press_state[1] = 1;
          break;
        }
        case SDLK_2:{
           key_press_state[2] = 1;
           break;
        }
        case SDLK_3:{
           key_press_state[3] = 1;
           break;
        }
        case SDLK_4:{
           key_press_state[12] = 1;
           break;
        }
        case SDLK_q:{
           key_press_state[4] = 1;
                      break;
        }
        case SDLK_w:{
           key_press_state[5] = 1;
                      break;
        }
        case SDLK_e:{
           key_press_state[6] = 1;
                      break;
        }
        case SDLK_r:{
           key_press_state[13] = 1;
                      break;
        }
         case SDLK_a:{
           key_press_state[7] = 1;
                   break;
        }
        case SDLK_s:{
          key_press_state[8] = 1;
                   break;
        }
        case SDLK_d:{
            key_press_state[9] = 1;
                   break;
        }
        case SDLK_f:{
           key_press_state[14] = 1;
                   break;
        }
         case SDLK_z:{
           key_press_state[10] = 1;
                   break;
        }
        case SDLK_x:{
            key_press_state[0] = 1;
                   break;
        }
        case SDLK_c:{
            key_press_state[11] = 1;
                   break;
        }
        case SDLK_v:{
           key_press_state[15] = 1;
                   break;
        }
      }
      return 1;
    }else
    if(e.type == SDL_KEYUP)
    {
      switch(e.key.keysym.sym)
      {
        case SDLK_1:{
          key_press_state[1] = 0;
          break;
        }
        case SDLK_2:{
           key_press_state[2] = 0;
           break;
        }
        case SDLK_3:{
           key_press_state[3] = 0;
           break;
        }
        case SDLK_4:{
           key_press_state[12] = 0;
           break;
        }
        case SDLK_q:{
           key_press_state[4] = 0;
                      break;
        }
        case SDLK_w:{
           key_press_state[5] = 0;
                      break;
        }
        case SDLK_e:{
           key_press_state[6] = 0;
                      break;
        }
        case SDLK_r:{
           key_press_state[13] = 0;
                      break;
        }
         case SDLK_a:{
           key_press_state[7] = 0;
                   break;
        }
        case SDLK_s:{
          key_press_state[8] = 0;
                   break;
        }
        case SDLK_d:{
            key_press_state[9] = 0;
                   break;
        }
        case SDLK_f:{
           key_press_state[14] = 0;
                   break;
        }
         case SDLK_z:{
           key_press_state[10] = 0;
                   break;
        }
        case SDLK_x:{
            key_press_state[0] = 0;
                   break;
        }
        case SDLK_c:{
            key_press_state[11] = 0;
                   break;
        }
        case SDLK_v:{
           key_press_state[15] = 0;
                   break;
        }
      }
      return 1;
    }else
    if(e.type == SDL_QUIT)
    {
      destroy_game_window();
      exit(0);
    }else{
      return 0;
    }
  }
  return 0;
}