#include <gb/gb.h>

#include "MyTileSet.c"
#include "MyTileMap.c"
#include "Smile.c"

#define TILE_SIZE 8
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define GAME_FLOOR (SCREEN_HEIGHT - 16)
#define PAST_SKY ((TILE_SIZE * 2) - 1)
#define PAST_CLOUDS ((TILE_SIZE * 5) - 1)
#define PAST_PYRAMIDS ((TILE_SIZE * 15) - 1)

typedef unsigned char UINT8;
typedef char INT8;

UINT8 scroll_sky, scroll_clouds, scroll_pyramids, scroll_sidewalk;
UINT8 cycle;

UINT8 player_pos[] = {80 - 8, GAME_FLOOR};


void parallaxScroll()
{
    if (LYC_REG == 0x00)
    {
        move_bkg(scroll_sky, 0);
        LYC_REG = PAST_SKY;
    }
    else if (LYC_REG == PAST_SKY)
    {
        move_bkg(scroll_clouds, 0);
        LYC_REG = PAST_CLOUDS;
    }
    else if (LYC_REG == PAST_CLOUDS)
    {
        move_bkg(scroll_pyramids, 0);
        LYC_REG = PAST_PYRAMIDS;
    }
    else if (LYC_REG == PAST_PYRAMIDS)
    {
        move_bkg(scroll_sidewalk, 0);
        LYC_REG = 0x00;
    }
}

void moveBackground(const UINT8 i, const INT8 direction)
{
    scroll_sky += (i % 4 == 0) ? (1 * direction) : 0;
    scroll_clouds += (i % 3 == 0) ? (1 * direction) : 0;
    scroll_pyramids += (i % 4 == 0) ? (1 * direction) : 0;
    scroll_sidewalk += (1 * direction);
}

inline void movePlayer()
{
    // The move_sprite function offests the x value by 8
    // and the y value by 16
    move_sprite(0, player_pos[0] + 8, player_pos[1] + 16);
}

void handleInput(const UINT8 cycle)
{
    UINT8 input = joypad();
    if (input & J_LEFT)
    {
        moveBackground(cycle, -1);
    }
    if (input & J_RIGHT)
    {
        moveBackground(cycle, 1);
    }
    if (input & J_UP)
    {
        player_pos[1]--; 
    }
    if (input & J_DOWN)
    {
        player_pos[1]++;
    }
}

void init()
{
    set_bkg_data(0, 13, MyTileSet);
    set_bkg_tiles(0, 0, MyTileMapWidth, MyTileMapHeight, MyTileMap);

    STAT_REG = 0x45;
    LYC_REG = 0x00;

    disable_interrupts();
    add_LCD(parallaxScroll);
    enable_interrupts();

    set_interrupts(VBL_IFLAG | LCD_IFLAG);

    DISPLAY_ON;

    SPRITES_8x8;

    HIDE_WIN;
    SHOW_SPRITES;
    SHOW_BKG;

    set_sprite_data(0, 0, smile);
	set_sprite_tile(0, 0);
}

void main(void)
{
    init();
    while(1)
    {
        handleInput(cycle);
        movePlayer();
        ++cycle;
        if (cycle == 12) { cycle = 0; } // Used to move background in fractions of steps
        wait_vbl_done();
    }
}