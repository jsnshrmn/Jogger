#include <gb/gb.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../res/level_map.h"
#include "../res/level_tiles.h"
#include "../res/player_sprite.h"

UINT8 player_position[2];
INT8 player_velocity[2];
UINT8 player_frame;
bool player_frame_op;

void init_gfx() {
    // Turn the display on
    DISPLAY_ON;
	// Turn the background map on to make it visible
    SHOW_BKG;
    // Load Background tiles and then map
    set_bkg_data(0, 79u, level_tiles);
    set_bkg_tiles(0, 0, 20u, 18u, level_map);
}

void init_player() {
    // Starting position
    player_position[0]=80;
    player_position[1]=136;
    // Starting animation frame
    player_frame=0;
    player_frame_op=1;
    // Enable large sprites
    SPRITES_8x16;
    // Load player sprite
    // The data is based on 8x8 sprites, so remember to multiply the number if using larger sprites
    set_sprite_data(0, 40, player_sprite);
    set_sprite_tile(0, player_frame);
    move_sprite(0, player_position[0], player_position[1]);

    // Starting velocity
    player_velocity[0]=0;
    player_velocity[1]=0;

    SHOW_SPRITES;
}

void animate_player_right() {
    if (player_frame == 20 ) {
        player_frame_op = false;
    } else if (player_frame == 1) {
        player_frame_op = true;
    }

    if (player_frame_op == true) {
        ++player_frame;
    } else {
        --player_frame;
    }
}

void animate_player_left() {
    if (player_frame == 39 ) {
        player_frame_op = false;
    } else if (player_frame == 22) {
        player_frame_op = true;
    }

    if (player_frame_op == true) {
        ++player_frame;
    } else {
        --player_frame;
    }
}

void move_player() {
        // Movement is broken up this way to allow for easy diagonal motion
        // Heavily inspired by:
        // https://laroldsjubilantjunkyard.com/tutorials/how-to-make-a-gameboy-game/joypad-input/

        // Vertical
        // Move up
        if (joypad() & J_UP && player_position[1] > 9 ) {
            player_velocity[1]=-1;
        // Move down
        } else if (joypad() & J_DOWN && player_position[1] < 136) {
            player_velocity[1]=1;
        // Stop vertical
        } else {
            player_velocity[1]=0;
        }

        // Horizontal
        // Move right
        if (joypad() & J_RIGHT) {
            animate_player_right();
            if (player_position[0] < 156) {
                player_velocity[0]=1;
            } else {
                player_velocity[0]=0;
            }
        // Move left
        } else if (joypad() & J_LEFT) {
            animate_player_left();
            if (player_position[0] > 4) {
                player_velocity[0]=-1;
            } else {
                player_velocity[0]=0;
            }
        // Stop horizontal
        } else {
            player_velocity[0]=0;
            player_frame = 0;
            set_sprite_tile(0, player_frame);
        }

        // Apply velocity
        player_position[0]+=player_velocity[0];
        player_position[1]+=player_velocity[1];

        // Position the first sprite at our player_position[0] and player_position[1]
        // All sprites are render 8 pixels to the left of their x position and 16 pixels ABOVE their actual y position
        // This means an object rendered at 0,0 will not be visible
        // x+4 and y+8 should center the 8x16 tile at our x and y position
        set_sprite_tile(0, player_frame);
        move_sprite(0,player_position[0]+4,player_position[1]+8);
}

void main(void)
{
	init_gfx();
    init_player();

    // Loop forever
    while(1) {


		// Game main loop processing goes here
        move_player();

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
