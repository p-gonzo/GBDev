#include <gb/gb.h>
#include "MyTileSet.c"
#include "MyTileMap.c"

const unsigned char TILE_SIZE = 8;
unsigned char scroll_sky, scroll_clouds, scroll_pyramids, scroll_sidewalk, scroll_wall;
unsigned char cycle;

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
        LYC_REG = TILE_SIZE * 16 - 2;
    }
    else if (LYC_REG == (TILE_SIZE * 16 - 2))
    {
        move_bkg(scroll_sidewalk, 0);
        LYC_REG = TILE_SIZE * 17 - 2;
    }
    else if (LYC_REG == (TILE_SIZE * 17 - 2))
    {
        move_bkg(scroll_wall, 0);
        LYC_REG = 0x00;
    }
}

void handleParalaxStep(const unsigned char i)
{
    scroll_sky += (i % 3 == 0) ? 1 : 0;
    scroll_clouds += (i % 2 == 0) ? 1 : 0;
    scroll_pyramids += 1;
    scroll_sidewalk += 2;
    scroll_wall += 3;
}

void main(void)
{
    set_bkg_data(0, 10, MyTileSet);
    set_bkg_tiles(0, 0, MyTileMapWidth, MyTileMapHeight, MyTileMap);

    STAT_REG = 0x45;
    LYC_REG = 0x00;

    disable_interrupts();
    add_LCD(parallaxScroll);
    enable_interrupts();

    set_interrupts(VBL_IFLAG | LCD_IFLAG);

    SHOW_BKG;
    DISPLAY_ON;

    while(1)
    {
        handleParalaxStep(cycle++);
        if (cycle == 10) { cycle = 0; }
        wait_vbl_done();
    }
}