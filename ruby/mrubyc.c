#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>

#define SCREENXRES 320          // Screen width
#define SCREENYRES 240          // Screen height
#define CENTERX SCREENXRES/2    // Center of screen on x 
#define CENTERY SCREENYRES/2    // Center of screen on y
#define MARGINX 2                // left margin
#define MARGINY 160              // bottom margin
#define FONTSIZE 8 * 7           // Text Field Height
DISPENV disp[2];                 // Double buffered DISPENV and DRAWENV
DRAWENV draw[2];
short db = 0;                      // index of which buffer is used, values 0, 1

void display_init(void)
{
    ResetGraph(0);                 // Initialize drawing engine with a complete reset (0)
    SetDefDispEnv(&disp[0], 0, 0         , SCREENXRES, SCREENYRES);     // Set display area for both &disp[0] and &disp[1]
    SetDefDispEnv(&disp[1], 0, SCREENYRES, SCREENXRES, SCREENYRES);     // &disp[0] is on top  of &disp[1]
    SetDefDrawEnv(&draw[0], 0, SCREENYRES, SCREENXRES, SCREENYRES);     // Set draw for both &draw[0] and &draw[1]
    SetDefDrawEnv(&draw[1], 0, 0         , SCREENXRES, SCREENYRES);     // &draw[0] is below &draw[1]
    SetDispMask(1);                 // Display on screen    
    setRGB0(&draw[0], 50, 50, 50); // set color for first draw area
    setRGB0(&draw[1], 50, 50, 50); // set color for second draw area
    draw[0].isbg = 1;               // set mask for draw areas. 1 means repainting the area with the RGB color each frame 
    draw[1].isbg = 1;
    PutDispEnv(&disp[db]);          // set the disp and draw environnments
    PutDrawEnv(&draw[db]);
    FntLoad(960, 0);                // Load font to vram at 960,0(+128)
    FntOpen(MARGINX, SCREENYRES - MARGINY - FONTSIZE, SCREENXRES - MARGINX * 2, FONTSIZE, 0, 280 ); // FntOpen(x, y, width, height,  black_bg, max. nbr. chars
}

void display(void)
{
    DrawSync(0);                    // Wait for all drawing to terminate
    VSync(0);                       // Wait for the next vertical blank
    PutDispEnv(&disp[db]);          // set alternate disp and draw environnments
    PutDrawEnv(&draw[db]);  
    db = !db;                       // flip db value (0 or 1)
}

#include "mrubyc.h"
#include "value.h"
#include "ruby.c" // file created with mrbc

#define MRBC_MEMORY_SIZE (1024*40)
static uint8_t memory_pool[MRBC_MEMORY_SIZE];

int main(void)
{
  hal_init();
  mrbc_init_alloc(memory_pool, MRBC_MEMORY_SIZE);
  mrbc_init_global();
  mrbc_init_class();

  mrbc_vm *vm = mrbc_vm_open(NULL);

  if( vm == NULL ) {
    printf("VM open failed\n");
    return 1;
  }

  if( mrbc_load_mrb(vm, mrbbuf) != 0 ) {
  	printf("Failed loading mrb!\n");
    return 1;

  }
  mrbc_vm_begin(vm);
  int status = mrbc_vm_run(vm);
  printf("VM run status: %d\n", status);

  mrb_value result = vm->regs[0];

  display_init();
  if( result.tt == MRBC_TT_STRING ) {
    const char *str = (const char *)result.instance + 16;
    printf("Returned string: %s\n", str);
    while (1) {
      FntPrint(str);
      FntFlush(-1);
      display();
    }
  } else {
    printf("Returned value is not a string.\n");
  }

  mrbc_vm_end(vm);
  mrbc_vm_close(vm);
  return 0;
}
