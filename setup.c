///// METASPRITES

#define NUM_ACTORS 2
#define TILE_SIZE 16

#define TILE_SIZE_BIT 4
#define FP_BITS	4

#define MAP_WDT 16
#define MAP_WDT_BIT 4
#define MAP_HGT 15

#define SURFACE 0x02
#define CUTTING_BOARD 0x04
#define CRATE 0x0A
#define POT 0x0f
#define BARRIER_1 0x00
#define BARRIER_2 0x0E
#define FLOOR 0x01

#define MAP_ADR(x,y)	((((y)-2)<<MAP_WDT_BIT)|(x))

const unsigned char solids[] = { 
  BARRIER_1, BARRIER_2, POT,
  CUTTING_BOARD, SURFACE, CRATE
};

const unsigned char interactable[] = {
  CUTTING_BOARD, SURFACE, CRATE, POT
};

// player is just a black square for now
const unsigned char player[] = {
    0, 0, 0x15, 0,
    0, 8, 0x15, 0,
    8, 0, 0x15, 0,
    8, 8, 0x15, 0,
    128};

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x03,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,	// background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};