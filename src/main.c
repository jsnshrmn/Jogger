#include <gb/gb.h>
#include <stdint.h>
#include "../res/level_map.h"
#include "../res/level_tiles.h"
#include "../res/player_sprite.h"

UINT8 spriteX,spriteY;
INT8 velocityX,velocityY;

void init_gfx() {
    // Turn the display on
    DISPLAY_ON;
	// Turn the background map on to make it visible
    SHOW_BKG;
    // Load Background tiles and then map
    set_bkg_data(0, 79u, level_tiles);
    set_bkg_tiles(0, 0, 20u, 18u, level_map);
}

void init_sprites() {
    // Starting position
    spriteX=80;
    spriteY=136;
    // Enable large sprites
    SPRITES_8x16;
    // Load player sprite
    set_sprite_data(0, 8, player_sprite);
    set_sprite_tile(0, 0);
    move_sprite(0, spriteX, spriteY);
    SHOW_SPRITES;

    // Starting velocity
    velocityX=0;
    velocityY=0;
}

void move_sprites() {
        // Movement is broken up this way to allow for easy diagonal motion
        // Heavily inspired by:
        // https://laroldsjubilantjunkyard.com/tutorials/how-to-make-a-gameboy-game/joypad-input/

        // Vertical
        // Move up
        if (joypad() & J_UP && spriteY > 8 ) {
            velocityY=-1;
        // Move down
        } else if (joypad() & J_DOWN && spriteY < 136) {
            velocityY=1;
        // Stop vertical
        } else {
            velocityY=0;
        }

        // Horizontal
        // Move right
        if (joypad() & J_RIGHT && spriteX < 156) {
            velocityX=1;
        // Move left
        } else if (joypad() & J_LEFT && spriteX > 4) {
            velocityX=-1;
        // Stop horizontal
        } else {
            velocityX=0;
        }

        // Apply velocity
        spriteX+=velocityX;
        spriteY+=velocityY;

        // Position the first sprite at our spriteX and spriteY
        // All sprites are render 8 pixels to the left of their x position and 16 pixels ABOVE their actual y position
        // This means an object rendered at 0,0 will not be visible
        // x+4 and y+8 should center the 8x16 tile at our x and y position
        move_sprite(0,spriteX+4,spriteY+8);
}

void main(void)
{
	init_gfx();
    init_sprites();

    // Loop forever
    while(1) {


		// Game main loop processing goes here
        move_sprites();

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
