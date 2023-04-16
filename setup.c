/*
#include "neslib.h"
#include <string.h>
//#resource "pt.chr"
//#link "map.s"
#include "nam.h"
#include "vrambuf.h"
//#link "vrambuf.c"*/

// constants
#define NUM_ACTORS 2
#define NUM_POTS 2
#define TILE_SIZE 16
#define NUM_SOLIDS 9
#define NUM_CRATES 3
#define MAP_WDT 16
#define MAP_HGT 15
#define HDBL_OFFSET 16

// bit sizes
#define TILE_SIZE_BIT 4
#define FP_BITS 4
#define MAP_WDT_BIT 4

// sprite map
#define SURFACE 0x10
#define CUTTING_BOARD 0x12
#define FISH_CRATE 0x16
#define RICE_CRATE 0x18
#define NORI_CRATE 0x1A
#define POT 0x14
#define BARRIER_1 0x02
#define BARRIER_2 0x01
#define FLOOR 0x01
#define SERVE 0x1E

// macros
#define MAP_ADR(x,y) ((((y)-2)<<MAP_WDT_BIT)|(x))
#define DEF_METASPRITE_2x2(name,code,pal)\
const unsigned char name[]={\
        0,      0,      (code)+0,   pal, \
        8,      0,      (code)+1,   pal, \
        0,      8,      (code)+16,   pal, \
        8,      8,      (code)+17,   pal, \
        128};

#define DEF_METASPRITE_1x2(name,code,pal)\
const unsigned char name[]={\
        0,      0,      (code)+0,   pal, \
        8,      0,      (code)+1,   pal, \
        128};

#define DEF_METASPRITE_1x1(name,code,pal)\
const unsigned char name[]={\
        0,      8,      (code)+1,   pal, \
        128};

DEF_METASPRITE_1x2(cooked_0, 0x50, 0)
DEF_METASPRITE_1x2(cooked_1, 0x60, 0)
DEF_METASPRITE_1x2(cooked_2, 0x70, 0)
DEF_METASPRITE_1x2(cooked_3, 0x80, 0)
DEF_METASPRITE_1x2(cooked_4, 0x90, 0)
DEF_METASPRITE_1x2(score_sushi, 0x74, 0)

DEF_METASPRITE_1x1(zero, 0xA8, 0);
DEF_METASPRITE_1x1(one, 0x9F, 0);
DEF_METASPRITE_1x1(two, 0xA0, 0);
DEF_METASPRITE_1x1(three, 0xA1, 0);
DEF_METASPRITE_1x1(four, 0xA2, 0);
DEF_METASPRITE_1x1(five, 0xA3, 0);
DEF_METASPRITE_1x1(six, 0xA4, 0);
DEF_METASPRITE_1x1(seven, 0xA5, 0);
DEF_METASPRITE_1x1(eight, 0xA6, 0);
DEF_METASPRITE_1x1(nine, 0xA7, 0);

const unsigned char* const boil_sequence[5] = {
  cooked_0,cooked_1,cooked_2,cooked_3,cooked_4
};

const unsigned char* const count_sequence[10] = {
  zero,one,two,three,four,five,six,seven,eight,nine
};

typedef struct Pot {
  bool isOn; // is Pot boiling?
  unsigned char px; // x coordinate on map
  unsigned char py; // y coordinate on map
  unsigned int frames; // frame count of boiling time
  unsigned int timer; // timer display time
} Pot;

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = {
  0x03, // screen color

  0x27,0x11,0x12,0x00, // background palette 0
  0x1C,0x20,0x2C,0x00, // background palette 1
  0x00,0x10,0x20,0x00, // background palette 2
  0x06,0x16,0x26,0x00, // background palette 3

  0x08,0x01,0x16,0x00, // sprite palette 0
  0x30,0x09,0x16,0x00, // sprite palette 1
  0x20,0x11,0x16,0x00, // sprite palette 2
  0x30,0x2A,0x09 // sprite palette 3
};

DEF_METASPRITE_2x2(player_spr, 0x36, 0)
DEF_METASPRITE_2x2(player1_spr, 0x38, 0)
DEF_METASPRITE_2x2(player2_spr, 0x3A, 0)
DEF_METASPRITE_2x2(rice_spr, 0x32, 0)
DEF_METASPRITE_2x2(fish_spr, 0x30, 0)
DEF_METASPRITE_2x2(nori_spr, 0x34, 0)
DEF_METASPRITE_2x2(prepped_fish_spr, 0x52, 0)
DEF_METASPRITE_2x2(prepped_rice_spr, 0x54, 0)
DEF_METASPRITE_2x2(rice_fish_spr, 0x72, 0)
DEF_METASPRITE_2x2(highlight_spr, 0x1c, 0)
DEF_METASPRITE_2x2(sushi_spr, 0x74, 0)
DEF_METASPRITE_2x2(nori_rice_spr, 0x76, 0)
DEF_METASPRITE_2x2(nori_fish_spr, 0x78, 0)

const unsigned char* const run_sequence[3] = {
  player_spr, player1_spr, player_spr
};

const unsigned char solids[NUM_SOLIDS] = {
  BARRIER_1, BARRIER_2, POT,
  CUTTING_BOARD, SURFACE, RICE_CRATE, FISH_CRATE, NORI_CRATE, SERVE
};

const unsigned char interactable[] = {
  CUTTING_BOARD, SURFACE, NORI_CRATE, FISH_CRATE, RICE_CRATE, POT
};

const unsigned char crates[] = {
  NORI_CRATE, FISH_CRATE, RICE_CRATE
};

const unsigned char player_pal[] = {0x01, 0x03};