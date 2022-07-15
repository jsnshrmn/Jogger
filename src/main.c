#include "../res/level_map.h"
#include "../res/level_tiles.h"
#include "../res/player_sprite.h"
#include <gb/gb.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

UINT8 tick_counter;
UINT8 player_position[2];
INT8 player_velocity[2];
UINT8 player_frame;
UINT8 player_stamina;
uint8_t joypad_state;

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
    player_position[0] = 80;
    player_position[1] = 136;
    // Starting animation frame
    player_frame = 0;
    // Starting player stamina
    player_stamina = 255;
    // Enable large sprites
    SPRITES_8x16;
    // Load player sprite
    // The data is based on 8x8 sprites, so remember to multiply the number if
    // using larger sprites
    set_sprite_data(0, 40, player_sprite);
    set_sprite_tile(0, player_frame);
    move_sprite(0, player_position[0], player_position[1]);

    // Starting velocity
    player_velocity[0] = 0;
    player_velocity[1] = 0;

    SHOW_SPRITES;
}

void animate_player_horizontal() {
    if (player_frame > 10) {
        player_frame = 2;
    }
    if ((tick_counter % 2) == 0) {
        ++player_frame;
    }
}

void animate_player_up() {
    if (player_frame < 13 || player_frame > 16) {
        player_frame = 13;
    }
    if ((tick_counter % 5) == 0) {
        ++player_frame;
    }
}

void animate_player_down() {
    if (player_frame < 18 || player_frame > 22) {
        player_frame = 18;
    }
    if ((tick_counter % 5) == 0) {
        ++player_frame;
    }
}

void animate_player_right() {
    animate_player_horizontal();
    set_sprite_prop(0, 0);
}

void animate_player_left() {
    animate_player_horizontal();
    // Mirror the sprite for moving left
    set_sprite_prop(0, S_FLIPX);
}

void player_gain_stamina() {
    if (player_stamina < 255 && (tick_counter % 2) == 0) {
        ++player_stamina;
    }
}

void player_lose_stamina() {
    if (player_stamina > 0) {
        --player_stamina;
    }
}

void animate_player_catch_breath() {
    if (player_frame < 24) {
        player_frame = 24;
    } else if (player_frame > 34) {
        player_frame = 24;
    }
    if ((tick_counter % 8) == 0) {
        ++player_frame;
    }
}

void animate_player_tap_foot() {
    if (player_frame < 36) {
        player_frame = 36;
    } else if (player_frame > 38) {
        player_frame = 36;
    } else {
        // player_frame = 0;
    }
    if ((tick_counter % 48) == 0) {
        ++player_frame;
    }
}

void player_stop() {
    if (player_stamina < 255) {
        animate_player_catch_breath();
    } else {
        animate_player_tap_foot();
        // animate_player_check_watch();
    }
    player_gain_stamina();
}

void move_player() {
    // Movement is broken up this way to allow for easy diagonal motion
    // Initially inspired by:
    // https://laroldsjubilantjunkyard.com/tutorials/how-to-make-a-gameboy-game/joypad-input/

    // Vertical
    // Move up
    if (joypad_state & J_UP) {
        // Favor horizontal animations since they have more frames
        if (!(joypad_state & J_LEFT || joypad_state & J_RIGHT)) {
            animate_player_up();
        }
        player_lose_stamina();
        if (player_position[1] > 9) {
            player_velocity[1] = -1;
        } else {
            player_velocity[1] = 0;
        }
        // Move down
    } else if (joypad_state & J_DOWN) {
        // Favor horizontal animations since they have more frames
        if (!(joypad_state & J_LEFT || joypad_state & J_RIGHT)) {
            animate_player_down();
        }
        player_lose_stamina();
        if (player_position[1] < 136) {
            player_velocity[1] = 1;
        } else {
            player_velocity[1] = 0;
        }
        // Stop vertical
    } else {
        player_velocity[1] = 0;
    }

    // Horizontal
    // Move right
    if (joypad_state & J_RIGHT) {
        animate_player_right();
        player_lose_stamina();
        if (player_position[0] < 156) {
            player_velocity[0] = 1;
        } else {
            player_velocity[0] = 0;
        }
        // Move left
    } else if (joypad_state & J_LEFT) {
        animate_player_left();
        player_lose_stamina();
        if (player_position[0] > 4) {
            player_velocity[0] = -1;
        } else {
            player_velocity[0] = 0;
        }
        // Stop horizontal
    } else {
        player_velocity[0] = 0;
    }

    // Reset to initial player frame
    if (!(joypad_state & J_UP || joypad_state & J_DOWN ||
          joypad_state & J_RIGHT || joypad_state & J_LEFT)) {
        player_stop();
    }

    // Apply velocity
    player_position[0] += player_velocity[0];
    player_position[1] += player_velocity[1];

    // Position the first sprite at our player_position[0] and
    // player_position[1] All sprites are render 8 pixels to the left of their x
    // position and 16 pixels ABOVE their actual y position This means an object
    // rendered at 0,0 will not be visible x+4 and y+8 should center the 8x16
    // tile at our x and y position
    set_sprite_tile(0, player_frame);
    move_sprite(0, player_position[0] + 4, player_position[1] + 8);
}

void main(void) {
    tick_counter = 0;
    init_gfx();
    init_player();

    // Loop forever
    while (1) {
        // Game main loop processing goes here
        // Only check the joypad state once per tick, per the docs
        // https://gbdk-2020.github.io/gbdk-2020/docs/api/gb_8h.html#a176c477d759b814664785f3a0ad5e253
        joypad_state = joypad();
        move_player();

        ++tick_counter;
        // Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
