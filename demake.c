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
byte actor_px[NUM_ACTORS];
byte actor_py[NUM_ACTORS];
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

static unsigned char ptr, spr;
static unsigned char held, prev_item;
static unsigned int i16;
static unsigned char i, j, k;
static unsigned char p_i;
static unsigned char px, py;
static unsigned int vel = 2;

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

void draw_holdable(int x, int y, int holdable_enum)
{
  switch (holdable_enum)
  {
    oam_off = 0;
  case FISH:
    oam_meta_spr_pal(x, y, 0x02, fish_spr);
    break;
  case NORI:
    oam_meta_spr_pal(x, y, 0x03, nori_spr);
    break;
  case RICE:
    oam_meta_spr_pal(x, y, 0x02, rice_spr);
    break;
  case PREPPED_FISH:
    oam_meta_spr_pal(x, y, 0x02, prepped_fish_spr);
    break;
  case PREPPED_RICE:
    oam_meta_spr_pal(x, y, 0x02, prepped_rice_spr);
    break;
  case FISH_RICE:
    oam_meta_spr_pal(x, y, 0x02, rice_fish_spr);
    break;
  case NORI_RICE:
    oam_meta_spr_pal(x, y, 0x03, nori_rice_spr);
    break;
  case NORI_FISH:
    oam_meta_spr_pal(x, y, 0x01, nori_fish_spr);
    break;
  case SUSHI:
    oam_meta_spr_pal(x, y, 0x01, sushi_spr);
    break;
  default:
    break;
  }
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

  for (p_i = 0; p_i < NUM_ACTORS; p_i++)
  {
    actor_x[p_i] = 120;
    actor_y[p_i] = 120;
    actor_dir[p_i] = RIGHT;
    holdable_x[p_i] = actor_x[p_i] - HDBL_OFFSET;
    holdable_y[p_i] = actor_y[p_i];
  }

  actor_x[1] = 100;
  actor_y[1] = 100;

  // loop forever
  while (true)
  {
    oam_off = 0;
    for (p_i = 0; p_i < NUM_ACTORS; p_i++)
    {
      pad_t = pad_trigger(p_i);
      pad = pad_state(p_i);

      // for collision detection w/ map representation
      actor_px[p_i] = actor_x[p_i] >> TILE_SIZE_BIT;
      actor_py[p_i] = actor_y[p_i] >> TILE_SIZE_BIT;

      // take input
      if (pad & PAD_LEFT && actor_x[p_i] > 0)
      {
        actor_dx[p_i] = -vel;
        actor_dir[p_i] = LEFT;

        holdable_x[p_i] = actor_x[p_i] + HDBL_OFFSET;
        holdable_y[p_i] = actor_y[p_i];
      }
      else if (pad & PAD_RIGHT && actor_x[p_i] < 240)
      {
        actor_dx[p_i] = vel;
        actor_px[p_i]++;
        actor_dir[p_i] = RIGHT;

        holdable_x[p_i] = actor_x[p_i] - HDBL_OFFSET;
        holdable_y[p_i] = actor_y[p_i];
      }
      else
      {
        actor_dx[p_i] = 0;
        actor_dir[p_i] = actor_dir[p_i];
      }

      if (pad & PAD_UP && actor_y[p_i] > 0)
      {
        actor_dy[p_i] = -vel;
        actor_dir[p_i] = UP;

        holdable_y[p_i] = actor_y[p_i] + HDBL_OFFSET;
        holdable_x[p_i] = actor_x[p_i];
      }
      else if (pad & PAD_DOWN && actor_y[p_i] < 212)
      {
        actor_dy[p_i] = vel;
        actor_py[p_i]++;
        actor_dir[p_i] = DOWN;

        holdable_y[p_i] = actor_y[p_i] - HDBL_OFFSET;
        holdable_x[p_i] = actor_x[p_i];
      }
      else
      {
        actor_dy[p_i] = 0;
        actor_dir[p_i] = actor_dir[p_i];
      }

      // draw player and player item
      oam_meta_spr_pal(actor_x[p_i], actor_y[p_i], player_pal[p_i], player_spr);
      draw_holdable(holdable_x[p_i], holdable_y[p_i], actor_holding[p_i]);

      // draw selected tile
      if (!tile_is_solid(map[MAP_ADR(actor_px[p_i], actor_py[p_i] + 2)]))
      {
        actor_x[p_i] += actor_dx[p_i];
        actor_y[p_i] += actor_dy[p_i];
      }
      else
      {
        oam_meta_spr_pal(actor_px[p_i] * 16, actor_py[p_i] * 16, 0x03, highlight_spr); // place playable tile

        // pick up / set down, use pad_trigger instead of pad_state
        // possibly modify what item player is holding
        if (pad_t & PAD_A)
        {
          
          held = actor_holding[p_i];
          ptr = MAP_ADR(actor_px[p_i], actor_py[p_i] + 2);
          prev_item = item_map[ptr];

          // default interaction: swap items with target
          actor_holding[p_i] = item_map[ptr];
          item_map[ptr] = held;

          // spec interaction: cut fish
          if (map[ptr] == CUTTING_BOARD && item_map[ptr] == FISH)
            item_map[ptr] = PREPPED_FISH;

          // spec interaction: boil rice
          else if (map[ptr] == POT && item_map[ptr] == RICE)
            item_map[ptr] = PREPPED_RICE;

          // spec interaction: fish rice
          else if ((prev_item == PREPPED_RICE && held == PREPPED_FISH) ||
                   (prev_item == PREPPED_FISH && held == PREPPED_RICE))
          {
            actor_holding[p_i] = NONE;
            item_map[ptr] = FISH_RICE;
          }

          else if ((prev_item == PREPPED_RICE && held == NORI) ||
                   (prev_item == NORI && held == PREPPED_RICE))
          {
            actor_holding[p_i] = NONE;
            item_map[ptr] = NORI_RICE;
          }

          else if ((prev_item == PREPPED_FISH && held == NORI) ||
                   (prev_item == NORI && held == PREPPED_FISH))
          {
            actor_holding[p_i] = NONE;
            item_map[ptr] = NORI_FISH;
          }

          else if ((prev_item == NORI_RICE && held == PREPPED_FISH) ||
                   (prev_item == PREPPED_FISH && held == NORI_RICE) ||
                   (prev_item == NORI && held == FISH_RICE) ||
                   (prev_item == FISH_RICE && held == NORI) ||
                   (prev_item == NORI_FISH && held == PREPPED_RICE) ||
                   (prev_item == PREPPED_RICE && held == NORI_FISH))
          {
            actor_holding[p_i] = NONE;
            item_map[ptr] = SUSHI;
          }

          // spec interaction: grab ingredient from crate if no items on crate or player
          if (held == NONE && prev_item == NONE)
          {
            switch (map[ptr])
            {
            case FISH_CRATE:
              actor_holding[p_i] = FISH;
              break;
            case NORI_CRATE:
              actor_holding[p_i] = NORI;
              break;
            case RICE_CRATE:
              actor_holding[p_i] = RICE;
              break;
            }
          }
          
        }
      }
    }

    // draw all loose items
    for (i = 0; i < MAP_HGT; i++)
    {
      for (j = 0; j < MAP_WDT; j++)
      {
        k = item_map[MAP_ADR(i, j + 2)];
        if (k != NONE)
        {
          draw_holdable(i << TILE_SIZE_BIT, j << TILE_SIZE_BIT, k);
        }
      }
    }


    // debug
    *(unsigned char *)0x00f0 = px;
    *(unsigned char *)0x00f2 = py;
    //*(unsigned char*)0x00ff = map[MAP_ADR(px, py)];
    *(unsigned char *)0x00ff = actor_holding[0];
    *(unsigned char *)0x00f1 = actor_dir[0];
    *(unsigned char *)0x00f4 = pad_state(0);
    oam_hide_rest(oam_off);

    ppu_wait_nmi();
  }
}