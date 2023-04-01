#include "neslib.h"
#include <string.h>

//#resource "sketch.chr"
//#link "map.s"
#include "nam.h"

#include "setup.c"

byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
// actor x/y deltas per frame (signed)
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];

byte actor_dir[NUM_ACTORS];


//static unsigned char debug[32];
static unsigned char map[MAP_WDT * MAP_HGT];
static unsigned char nameRow[32];
//static unsigned char tile[1];
static unsigned char ptr, spr;
static unsigned int i16;
static unsigned char i, j;
static unsigned char px, py;

typedef enum { LEFT, RIGHT, UP, DOWN } PlayerDirections;


char pad;
int future_x;
int future_y;


bool tile_is_solid(char tile){
  int i;
  for (i=0; i<6; i++){
    if(solids[i] == tile)
    	return true;
  }
  return false;
}

// main program
void main()
{
  // draw preloaded nametable
  vram_adr(NAMETABLE_A);
  vram_write(nam, 1024);
  pal_all(PALETTE);

  // now read vram and get map representation
  i16 = NAMETABLE_A+0x80;
  ptr = 0;
  
  for (i = 0; i < MAP_HGT; i++)
  {
    vram_adr(i16);
    vram_read(nameRow, 32);
    //vram_adr(i16);
    
    for (j = 0; j < MAP_WDT<<1; j+=2)
    {
      spr = nameRow[j];
      map[ptr] = spr;
      ptr++;
    }
    i16 += 64;
  }

  ppu_on_all();

  actor_x[0] = 120;
  actor_y[0] = 120;

  // loop forever
  while (true)
  {
    pad = pad_poll(0);

    // for collision detection w/ map representation
    px=actor_x[0]>>TILE_SIZE_BIT;
    py=actor_y[0]>>TILE_SIZE_BIT;
    
    // take input
    if (pad & PAD_LEFT && actor_x[0] > 0){
      actor_dx[0] = -2;
      actor_dir[0] = LEFT;
    }
    else if (pad & PAD_RIGHT && actor_x[0] < 240){
      actor_dx[0] = 2;
      px++;
      actor_dir[0] = RIGHT;
    } 
    else
      actor_dx[0] = 0;

    if (pad & PAD_UP && actor_y[0] > 0){
      actor_dy[0] = -2;
      actor_dir[0] = UP;
    }
    else if (pad & PAD_DOWN && actor_y[0] < 212){
      actor_dy[0] = 2;
      py++;
      actor_dir[0] = DOWN;
    } 
    else
      actor_dy[0] = 0;
    
    // update and draw
    oam_meta_spr(actor_x[0], actor_y[0], 0, player);
    if (!tile_is_solid(map[MAP_ADR(px, py)])){
      actor_x[0] += actor_dx[0];
      actor_y[0] += actor_dy[0];
    }
    
    // debug
    *(unsigned char*)0x00f0 = px;
    *(unsigned char*)0x00f2 = py;
    *(unsigned char*)0x00ff = map[MAP_ADR(px, py)];
    
    ppu_wait_frame();
  }
}
