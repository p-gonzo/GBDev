#include <gb/gb.h>

#include "MyTileSet.c"
#include "MyTileMap.c"
#include "Smile.c"

typedef unsigned char UINT8;
typedef char INT8;

const UINT8 TILE_SIZE = 8;
UINT8 scroll_sky, scroll_clouds, scroll_pyramids, scroll_sidewalk;
UINT8 cycle;
UINT8 player_pos[] = {80, 144};


void parallaxScroll()
{
    if (LYC_REG == 0x00)
    {
        move_bkg(scroll_sky, 0);
        LYC_REG = TILE_SIZE * 2;
    }
    else if (LYC_REG == (TILE_SIZE * 2))
    {
        move_bkg(scroll_clouds, 0);
        LYC_REG = TILE_SIZE * 5;
    }
    else if (LYC_REG == (TILE_SIZE * 5))
    {
        move_bkg(scroll_pyramids, 0);
        LYC_REG = TILE_SIZE * 15 - 2;
    }
    else if (LYC_REG == (TILE_SIZE * 15 - 2))
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
    move_sprite(0, player_pos[0], player_pos[1]);
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
        ++cycle;
        if (cycle == 12) { cycle = 0; } // Used to move background in fractions of steps
        wait_vbl_done();
    }
}