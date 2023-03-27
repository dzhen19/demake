/*
Read from controllers with pad_poll().
We also demonstrate sprite animation by cycling among
several different metasprites, which are defined using macros.
*/

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

#include "nam.h"


// link the pattern table into CHR ROM
//#link "chr_generic.s"

#include "setup.c"

//static unsigned char map[MAP_WDT * MAP_HGT];
const unsigned char palGame[16]={ 0x0f,0x11,0x32,0x30,0x0f,0x1c,0x2c,0x3c,0x0f,0x09,0x27,0x38,0x0f,0x11,0x21,0x31 };


// setup PPU and tables

void setup_graphics()
{
    // clear sprites
    oam_hide_rest(0);
    // set palette colors
    pal_all(PALETTE);
    // turn on PPU
    ppu_on_all();
}


// number of actors (4 h/w sprites each)
#define NUM_ACTORS 2

// actor x/y positions
byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
// actor x/y deltas per frame (signed)
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];

// main program
void main()
{
    char i; // actor index
    char oam_id;	// sprite ID
    char pad;	// controller flags
  
    // setup graphics
    setup_graphics();

    //oam_meta_spr(8, 8, 4, barrier);

    // initialize actors with random values
    for (i = 0; i < NUM_ACTORS; i++)
    {
        actor_x[i] = i * 32 + 128;
        actor_y[i] = i * 8 + 64;
        actor_dx[i] = 0;
        actor_dy[i] = 0;
    }
  
  
    // loop forever
    while (1)
    {
        //vram_adr(NAMETABLE_A);
        vram_unrle(nam);
        pal_bg(palGame); 	
        oam_clear();
      
        // set player 0/1 velocity based on controller
        for (i=0; i<2; i++) {
          // poll controller i (0-1)
          pad = pad_poll(i);
          // move actor[i] left/right
          if (pad&PAD_LEFT && actor_x[i]>0) actor_dx[i]=-2;
          else if (pad&PAD_RIGHT && actor_x[i]<240) actor_dx[i]=2;
          else actor_dx[i]=0;
          // move actor[i] up/down
          if (pad&PAD_UP && actor_y[i]>0) actor_dy[i]=-2;
          else if (pad&PAD_DOWN && actor_y[i]<212) actor_dy[i]=2;
          else actor_dy[i]=0;
        }


        // draw and move all actors
        for (i=0; i<NUM_ACTORS; i++) {
          byte runseq = actor_x[i] & 7;
          if (actor_dx[i] >= 0)
            runseq += 8;
          oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRunSeq[runseq]);
          actor_x[i] += actor_dx[i];
          actor_y[i] += actor_dy[i];
        }


        // hide rest of sprites
        // if we haven't wrapped oam_id around to 0
        if (oam_id!=0) oam_hide_rest(oam_id);

        // wait for next frame
        ppu_wait_frame();
    }
}
