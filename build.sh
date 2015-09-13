#!/bin/bash
gcc -o chip8 main.c ./core/chip8.c ./gui/game_window.c ./gui/key_press.c `sdl2-config --cflags --libs`
echo "Build Complete..."
