#include "neslib.h"
#include <string.h>

//#resource "pt.chr"
//#link "map.s"
#include "nam.h"

#include "setup.c"
#include "vrambuf.h"
//#link "vrambuf.c"

byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
byte holdable_x[NUM_ACTORS];
byte holdable_y[NUM_ACTORS];
// actor x/y deltas per frame (signed)
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];

byte actor_dir[NUM_ACTORS];
byte actor_playable[NUM_ACTORS];
byte actor_holding[NUM_ACTORS];

static unsigned char map[MAP_WDT * MAP_HGT];
static unsigned char item_map[MAP_WDT * MAP_HGT];

static unsigned char nameRow[32];
static char oam_id;

static unsigned char ptr, spr;
static unsigned int i16;
static unsigned char i, j, k;
static unsigned char px, py;
static unsigned int vel = 4;

typedef enum
{
  LEFT,
  RIGHT,
  UP,
  DOWN
} PlayerDirections;
typedef enum
{
  NONE,
  NORI,
  RICE,
  FISH,
  PREPPED_RICE,
  PREPPED_FISH,
  FISH_RICE,
  NORI_RICE,
  NORI_FISH,
  SUSHI
} PlayerHoldable;

char pad, pad_t;
int future_x;
int future_y;

bool tile_is_solid(char tile)
{
  int i;
  for (i = 0; i < NUM_SOLIDS; i++)
  {
    if (solids[i] == tile)
      return true;
  }
  return false;
}

int draw_holdable(int x, int y, int oam_id, int holdable_enum)
{
  int new_oam_id = oam_id;
  switch (holdable_enum)
  {
  case FISH:
    new_oam_id = oam_meta_spr(x, y, oam_id, fish_spr);
    break;
  case NORI:
    new_oam_id = oam_meta_spr(x, y, oam_id, nori_spr);
    break;
  case RICE:
    new_oam_id = oam_meta_spr(x, y, oam_id, rice_spr);
    break;
  case PREPPED_FISH:
    new_oam_id = oam_meta_spr(x, y, oam_id, prepped_fish_spr);
    break;
  case PREPPED_RICE:
    new_oam_id = oam_meta_spr(x, y, oam_id, prepped_rice_spr);
    break;
    case FISH_RICE:
      new_oam_id = oam_meta_spr(x, y, oam_id, rice_fish_spr);
      break;
  default:
    break;
  }
  return new_oam_id;
}

// main program
void main()
{
  // draw preloaded nametable
  vram_adr(NAMETABLE_A);
  vram_write(nam, 1024);
  pal_all(PALETTE);

  vrambuf_clear();
  set_vram_update(updbuf);

  // now read vram and get map representation
  // i16 = NAMETABLE_A+0x80;
  i16 = NTADR_A(0, 0);
  ptr = 0;

  for (i = 0; i < MAP_HGT; i++)
  {
    vram_adr(i16);
    vram_read(nameRow, 32);
    vram_adr(i16);

    for (j = 0; j < MAP_WDT << 1; j += 2)
    {
      spr = nameRow[j];
      map[ptr] = spr;
      // item_map[ptr] = spr;
      switch (spr)
      { /*
       case FISH_CRATE:
         item_map[ptr] = FISH;
         break;
       case NORI_CRATE:
         item_map[ptr] = NORI;
         break;
       case RICE_CRATE:
         item_map[ptr] = RICE;
         break;*/
      default:
        item_map[ptr] = NONE;
      }

      ptr++;
    }
    i16 += 64;
  }

  ppu_on_all();

  actor_x[0] = 120;
  actor_y[0] = 120;
  actor_dir[0] = RIGHT;
  holdable_x[0] = actor_x[0] - HDBL_OFFSET;
  holdable_y[0] = actor_y[0];
  // loop forever
  while (true)
  {
    pad_t = pad_trigger(0);
    pad = pad_state(0);

    // for collision detection w/ map representation
    px = actor_x[0] >> TILE_SIZE_BIT;
    py = (actor_y[0] >> TILE_SIZE_BIT);

    // take input
    if (pad & PAD_LEFT && actor_x[0] > 0)
    {
      actor_dx[0] = -vel;
      actor_dir[0] = LEFT;

      holdable_x[0] = actor_x[0] + HDBL_OFFSET;
      holdable_y[0] = actor_y[0];
    }
    else if (pad & PAD_RIGHT && actor_x[0] < 240)
    {
      actor_dx[0] = vel;
      px++;
      actor_dir[0] = RIGHT;

      holdable_x[0] = actor_x[0] - HDBL_OFFSET;
      holdable_y[0] = actor_y[0];
    }
    else
    {
      actor_dx[0] = 0;
      actor_dir[0] = actor_dir[0];
    }

    if (pad & PAD_UP && actor_y[0] > 0)
    {
      actor_dy[0] = -vel;
      actor_dir[0] = UP;

      holdable_y[0] = actor_y[0] + HDBL_OFFSET;
      holdable_x[0] = actor_x[0];
    }
    else if (pad & PAD_DOWN && actor_y[0] < 212)
    {
      actor_dy[0] = vel;
      py++;
      actor_dir[0] = DOWN;

      holdable_y[0] = actor_y[0] - HDBL_OFFSET;
      holdable_x[0] = actor_x[0];
    }
    else
    {
      actor_dy[0] = 0;
      actor_dir[0] = actor_dir[0];
    }

    // draw player and player item
    oam_id = oam_meta_spr(actor_x[0], actor_y[0], 0, player_spr);
    oam_id = draw_holdable(holdable_x[0], holdable_y[0], oam_id, actor_holding[0]);

    // draw all loose items
    for (i = 0; i < MAP_HGT; i++)
    {
      for (j = 0; j < MAP_WDT; j++)
      {
        k = item_map[MAP_ADR(i, j + 2)];
        if (k != NONE)
        {
          oam_id = draw_holdable(i << TILE_SIZE_BIT, j << TILE_SIZE_BIT, oam_id, k);
        }
      }
    }

    // draw selected tile
    if (!tile_is_solid(map[MAP_ADR(px, py + 2)]))
    {
      actor_x[0] += actor_dx[0];
      actor_y[0] += actor_dy[0];
      if (actor_playable[0] != 0)
        oam_hide_rest(actor_playable[0]); // remove former playable tile
    }
    else
    {
      // draw highlight
      actor_playable[0] = oam_id;
      oam_id = oam_meta_spr(px * 16, py * 16, oam_id, highlight_spr); // place playable tile

      // pick up / set down, use pad_trigger instead of pad_state
      // possibly modify what item player is holding
      if (pad_t & PAD_A)
      {
        char held = actor_holding[0];
        char prev_item;
        ptr = MAP_ADR(px, py + 2);
        prev_item = item_map[ptr];
        
        // default interaction: swap items with target
        actor_holding[0] = item_map[ptr];
        item_map[ptr] = held;

	// spec interaction: cut fish
        if (map[ptr] == CUTTING_BOARD && item_map[ptr] == FISH)
          item_map[ptr] = PREPPED_FISH;
        
        // spec interaction: boil rice
        if (map[ptr] == POT && item_map[ptr] == RICE)
           item_map[ptr] = PREPPED_RICE;
        
        // spec interaction: fish rice
        if ((prev_item == PREPPED_RICE && held == PREPPED_FISH) ||
            (prev_item == PREPPED_FISH && held == PREPPED_RICE)){
          actor_holding[0] = NONE;
          item_map[ptr] = FISH_RICE;
        }
	
        // spec interaction: grab ingredient from crate if no items on crate or player
        if (held == NONE && prev_item==NONE){
          switch(map[ptr]){
            case FISH_CRATE:
              actor_holding[0] = FISH;
              break;
            case NORI_CRATE:
              actor_holding[0] = NORI;
              break;
            case RICE_CRATE:
              actor_holding[0] = RICE;
              break;
          }
        }
          
        
        /*
        switch (held)
        {
        case FISH:
            if (highlighted_spr == CUTTING_BOARD)
              item_map[ptr];
            break;
        case NONE:
          // check if a crate is being highlighted
          if (item_map[ptr]!=NONE) break;
          switch (highlighted_spr)
          {
          case FISH_CRATE:
            item_map[ptr] = FISH;
            break;
          case NORI_CRATE:
            item_map[ptr] = NORI;
            break;
          case RICE_CRATE:
            item_map[ptr] = RICE;
            break;
          }
          break;
        }
        */
        
      }
    }

    // debug
    *(unsigned char *)0x00f0 = px;
    *(unsigned char *)0x00f2 = py;
    //*(unsigned char*)0x00ff = map[MAP_ADR(px, py)];
    *(unsigned char *)0x00ff = actor_holding[0];
    *(unsigned char *)0x00f1 = actor_dir[0];
    *(unsigned char *)0x00f3 = oam_id;
    *(unsigned char *)0x00f4 = pad;

    ppu_wait_frame();
  }
}