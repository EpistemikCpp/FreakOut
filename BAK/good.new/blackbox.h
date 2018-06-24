// BLACKBOX.H - Header file for demo game engine library

// watch for multiple inclusions
#ifndef __BLACKBOX_H
#define __BLACKBOX_H

#include <ddraw.h>  // NEED THIS FOR DIRECTDRAW VARIABLE TYPES

// initializes a direct draw struct
#define DD_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

/* NEED A GLOBAL LPDIRECTDRAWSURFACE4 TO USE AS THE DEFAULT PARAMETER
   IN THE GRAPHICS FUNCTIONS -- THIS PARAMETER NEEDS TO BE DECLARED 
   IN BLACKBOX.CPP AND REFERENCED AS AN EXTERN IN BLACKBOX.H 
   - FROM BLACKBOX.H IT IS VISIBLE IN FREAKOUT.CPP & FREAKMAIN.CPP */
extern LPDIRECTDRAWSURFACE4 lpddsback;  // dd back surface

// screen parameters
const int SCREEN_WIDTH  = 800 , // size of screen
          SCREEN_HEIGHT = 600 ,
          MAX_COLORS    = 256 ; // maximum colors (8 bits per pixel)

class Blackbox
{
 public:

 LPDIRECTDRAW4         lpdd;           // dd object
 LPDIRECTDRAWSURFACE4  lpddsprimary;   // dd primary surface
 LPDIRECTDRAWPALETTE   lpddpal;        // a pointer to the created dd palette   
 LPDIRECTDRAWCLIPPER   lpddclipper;    // dd clipper

 PALETTEENTRY          palette[MAX_COLORS];      // color palette
 PALETTEENTRY          save_palette[MAX_COLORS]; // used to save palettes
 
 DDSURFACEDESC2        ddsd;      // a direct draw surface description struct      
 DDBLTFX               ddbltfx;   // used to fill
 DDSCAPS2              ddscaps;   // a direct draw surface capabilities struct
 
 HRESULT               ddrval;      // result back from dd calls
 DWORD                 clock_start; // used for timing

// these define the general clipping rectangle
 int min_clip_x,  max_clip_x,                  
     min_clip_y,  max_clip_y;                  

// these are overwritten globally by DD_Init()
 int screen_width,     // width of screen
     screen_height,    // height of screen
     screen_bpp;       // bits per pixel 

// constructor with default values
Blackbox(int=SCREEN_HEIGHT, int=SCREEN_WIDTH, int=MAX_COLORS) ;

// DirectDraw functions
int DD_Init(HWND, int, int, int);
int DD_Shutdown(void);

LPDIRECTDRAWCLIPPER DD_Attach_Clipper( LPDIRECTDRAWSURFACE4 lpdds,
																			 int num_rects, LPRECT clip_list );
int DD_Flip(void);
int DD_Fill_Surface(LPDIRECTDRAWSURFACE4 lpdds, int color);

// general utility functions
DWORD Start_Clock(void);
DWORD Get_Clock(void);
DWORD (Wait_Clock)(DWORD count);

// graphics functions
void Define_palette(void) ;
int Draw_Rectangle( int x1, int y1, int x2, int y2, int color, 
   								  LPDIRECTDRAWSURFACE4 lpdds = lpddsback );
int Draw_Text_GDI( char *text, int x, int y, int color, 
									 LPDIRECTDRAWSURFACE4 lpdds = lpddsback);
  };
// class Blackbox

#endif
