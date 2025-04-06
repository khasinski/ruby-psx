#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
//#include "cube.c"
#include "diamond.c"

// Screen setup
#define SCREENXRES 320          // Screen width
#define SCREENYRES 240          // Screen height
#define CENTERX SCREENXRES/2    // Center of screen on x 
#define CENTERY SCREENYRES/2    // Center of screen on y
#define MARGINX 2                // left margin
#define MARGINY 160              // bottom margin
#define FONTSIZE 8 * 7           // Text Field Height
#define OTLEN       2048        // Maximum number of OT entries
#define PRIMBUFFLEN 32768       // Maximum number of POLY_GT3 primitives
DISPENV disp[2];                 // Double buffered DISPENV and DRAWENV
DRAWENV draw[2];
short db = 0;                      // index of which buffer is used, values 0, 1
u_long      ot[2][OTLEN];                   // Ordering table (contains addresses to primitives)
char        primbuff[2][PRIMBUFFLEN]; // Primitive list // That's our prim buffer
char * nextpri = primbuff[0];                       // Primitive counter

// Rotating cube
long    t, p, OTz, Flag;                // t == vertex count, p == depth cueing interpolation value, OTz ==  value to create Z-ordered OT, Flag == see LibOver47.pdf, p.143
POLY_G3 *poly = {0};                           // pointer to a POLY_G4
SVECTOR Rotate={ 232, 232, 0, 0 };                   // Rotation coordinates
VECTOR  Trans={ 0, 0, CENTERX * 1, 0 };     // Translation coordinates
                                        // Scaling coordinates
VECTOR  Scale={ ONE/2, ONE/2, ONE/2, 0 };     // ONE == 4096
MATRIX  Matrix={0};                     // Matrix data for the GTE

void display_init(void)
{
    ResetGraph(0);                 // Initialize drawing engine with a complete reset (0)
    InitGeom();
    SetGeomOffset(CENTERX, CENTERY);        // x, y offset
    SetGeomScreen(CENTERX);                 // Distance between eye and screen
    SetDefDispEnv(&disp[0], 0, 0         , SCREENXRES, SCREENYRES);     // Set display area for both &disp[0] and &disp[1]
    SetDefDispEnv(&disp[1], 0, SCREENYRES, SCREENXRES, SCREENYRES);     // &disp[0] is on top  of &disp[1]
    SetDefDrawEnv(&draw[0], 0, SCREENYRES, SCREENXRES, SCREENYRES);     // Set draw for both &draw[0] and &draw[1]
    SetDefDrawEnv(&draw[1], 0, 0         , SCREENXRES, SCREENYRES);     // &draw[0] is below &draw[1]
    SetDispMask(1);                 // Display on screen    
    setRGB0(&draw[0], 160, 26, 2); // set color for first draw area
    setRGB0(&draw[1], 160, 26, 2); // set color for second draw area

    draw[0].isbg = 1;               // set mask for draw areas. 1 means repainting the area with the RGB color each frame 
    draw[1].isbg = 1;
    PutDispEnv(&disp[db]);          // set the disp and draw environnments
    PutDrawEnv(&draw[db]);
    FntLoad(960, 0);                // Load font to vram at 960,0(+128)
    FntOpen(MARGINX, SCREENYRES - MARGINY - FONTSIZE, SCREENXRES - MARGINX * 2, FONTSIZE, 0, 280 ); // FntOpen(x, y, width, height,  black_bg, max. nbr. chars
}

#include "mrubyc.h"
#include "value.h"
#include "ruby.c" // file created with mrbc

#define MRBC_MEMORY_SIZE (1024*40)
static uint8_t memory_pool[MRBC_MEMORY_SIZE];

static void c_print_display(struct VM *vm, mrbc_value v[], int argc)
{
  FntPrint(GET_STRING_ARG(1));
  FntFlush(-1);
  SET_INT_RETURN(0);
}

static void c_update_display(struct VM *vm, mrbc_value v[], int argc)
{
  int progress = GET_INT_ARG(1);
  Rotate.vy += progress; // Pan
  Rotate.vx += progress; // Tilt
  // Clear the current OT
  ClearOTagR(ot[db], OTLEN);
  // Convert and set the matrixes
  RotMatrix(&Rotate, &Matrix);
  TransMatrix(&Matrix, &Trans);
  ScaleMatrix(&Matrix, &Scale);
  SetRotMatrix(&Matrix);
  SetTransMatrix(&Matrix);
  // Render the sample vector model
  t=0;
  // modelCube is a TMESH, len member == # vertices, but here it's # of triangle... So, for each tri * 3 vertices ...
  for (int i = 0; i < (modelCube.len*3); i += 3) {
    poly = (POLY_G3 *)nextpri;
    // Initialize the primitive and set its color values
    SetPolyG3(poly);
    setRGB0(poly, modelCube.c[i].r , modelCube.c[i].g   , modelCube.c[i].b);
    setRGB1(poly, modelCube.c[i+2].r, modelCube.c[i+2].g, modelCube.c[i+2].b);
    setRGB2(poly, modelCube.c[i+1].r, modelCube.c[i+1].g, modelCube.c[i+1].b);
    // Rotate, translate, and project the vectors and output the results into a primitive
    OTz  = RotTransPers(&modelCube_mesh[modelCube_index[t]]  , (long*)&poly->x0, &p, &Flag);
    OTz += RotTransPers(&modelCube_mesh[modelCube_index[t+2]], (long*)&poly->x1, &p, &Flag);
    OTz += RotTransPers(&modelCube_mesh[modelCube_index[t+1]], (long*)&poly->x2, &p, &Flag);
    // Sort the primitive into the OT
    OTz /= 3;
    if ((OTz > 0) && (OTz < OTLEN))
        AddPrim(&ot[db][OTz-2], poly);
    nextpri += sizeof(POLY_G3);
    t+=3;
  }

  mrbc_int_t num = mrbc_integer(v[1]);
  DrawSync(0);                    // Wait for all drawing to terminate
  VSync(0);                       // Wait for the next vertical blank
  PutDispEnv(&disp[db]);          // set alternate disp and draw environnments
  PutDrawEnv(&draw[db]);
  DrawOTag(&ot[db][OTLEN - 1]);
  db = !db;
  nextpri = primbuff[db];
  SET_INT_RETURN(0);
}

static void c_integer_add(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int_t num = mrbc_integer(v[1]);
  SET_INT_RETURN(v->i + num);
}

static void c_integer_subtract(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int_t num = mrbc_integer(v[1]);
  SET_INT_RETURN(v->i - num);
}

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

  display_init();

  mrbc_class *integer_cls = mrbc_get_class_by_name("Integer");
  mrbc_define_method(vm, integer_cls, "+", c_integer_add);
  mrbc_define_method(vm, integer_cls, "-", c_integer_subtract);
  mrbc_define_method(vm, mrbc_class_object, "print_display", c_print_display);
  mrbc_define_method(vm, mrbc_class_object, "update_display", c_update_display);

  mrbc_vm_begin(vm);
  int status = mrbc_vm_run(vm);
  printf("VM run status: %d\n", status);
  mrb_value result = vm->regs[0];
  mrbc_vm_end(vm);
  mrbc_vm_close(vm);
  return 0;
}
