#include <gb/gb.h>

#include "MyTileSet.c"
#include "MyTileMap.c"
#include "Player.c"

#define TRUE 1
#define FALSE 0
#define TILE_SIZE 8
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 16
#define GAME_FLOOR (SCREEN_HEIGHT - PLAYER_HEIGHT - 8)
#define PAST_SKY ((TILE_SIZE * 2) - 1)
#define PAST_CLOUDS ((TILE_SIZE * 5) - 1)
#define PAST_PYRAMIDS ((TILE_SIZE * 17) - 1)
#define JUMP_VELOCITY 8
#define GRAVITY 1

typedef unsigned char UINT8;
typedef char INT8;
typedef char BOOL;

UINT8 scroll_sky, scroll_clouds, scroll_pyramids, scroll_sidewalk;
UINT8 cycle;

UINT8 player_pos[] = {80 - 8, GAME_FLOOR};
UINT8 player_delta[] = {0, 0};
UINT8 playerDirection;

BOOL pressingJump = FALSE;
BOOL pressingLeft = FALSE;
BOOL pressingRight = FALSE;

BOOL playerOnGround = TRUE;
BOOL jumpedFromPress = FALSE;


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

void updateBackground()
{
    playerDirection= (pressingLeft ? -1 : (pressingRight ? 1 : 0));
    scroll_sky += (cycle % 24 == 0) ? 1 : 0;
    scroll_clouds += (cycle % 12 == 0) ? 1 : 0;
    scroll_pyramids += (cycle % 6 == 0) ? (1 * playerDirection) : 0;
    scroll_sidewalk += (cycle % 2 == 0) ? (1 * playerDirection) : 0;
}

inline void drawPlayer()
{
    if (pressingRight) {set_sprite_tile(0, 0);}
    if (pressingLeft) {set_sprite_tile(0, 2);}
    // The move_sprite function offests the x value by 8
    // and the y value by 16
    move_sprite(0, player_pos[0] + 8, player_pos[1] + 16);
}

void handleInput()
{
    UINT8 input = joypad();
    pressingLeft = (input & J_LEFT);
    pressingRight = (input & J_RIGHT);    
    if (input & J_A)
    {
        pressingJump = TRUE;
    }
    else
    {
        pressingJump = FALSE;
        jumpedFromPress = FALSE;
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

    SPRITES_8x16;

    HIDE_WIN;
    SHOW_SPRITES;
    SHOW_BKG;

    set_sprite_data(0, 8, Player);
	set_sprite_tile(0, 0);
}

void updatePlayer()
{
    if (playerOnGround && pressingJump && !jumpedFromPress)
    {
        player_delta[1] = JUMP_VELOCITY * -1;
        jumpedFromPress = TRUE;
    }
    player_pos[0] += player_delta[0];
    player_pos[1] += player_delta[1];
    playerOnGround = (player_pos[1] >= GAME_FLOOR) ? TRUE : FALSE;
    player_pos[1] = playerOnGround ? GAME_FLOOR : player_pos[1];
    player_delta[1] = playerOnGround ? 0 : player_delta[1] + GRAVITY;
}

void main(void)
{
    init();
    while(1)
    {
        handleInput();
        updateBackground();
        updatePlayer();
        drawPlayer();
        // Background tile set automatically drawn
        if (++cycle == 24) { cycle = 0; } // Used to move background in fractions of cycles
        wait_vbl_done();
    }
}