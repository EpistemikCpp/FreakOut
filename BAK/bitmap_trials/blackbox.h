// BLACKBOX.H - Header file for DirectX functions

// watch for multiple inclusions
#ifndef FREAKPROJ_BLACKBOX_H
#define FREAKPROJ_BLACKBOX_H

#include <ddraw.h>  // NEED THIS FOR DIRECTDRAW VARIABLE TYPES

// initialize a direct draw struct
#define DD_INIT_STRUCT(ddstruct) { memset( &ddstruct, 0, sizeof(ddstruct) ); \
                                   ddstruct.dwSize = sizeof(ddstruct); }

// color defines - from palette set in blackbox.cpp
#define BLACK     0
#define GREY      1
#define WHITE     2
#define PINK      3
#define RED      30
#define ORANGE   58
#define YELLOW   86
#define LGREEN  114
#define DGREEN  142
#define CYAN    170
#define BLUE    198
#define VIOLET  226
#define MAGENTA 254

/* need a GLOBAL LPDIRECTDRAWSURFACE4 to use as the default parameter
   in the graphics functions -- this parameter needs to be declared 
   in blackbox.cpp and referenced as an extern in blackbox.h 
   -> from blackbox.h it is visible in freakout.cpp & freakmain.cpp */
extern LPDIRECTDRAWSURFACE4 lpddsback ;  // dd back surface

// screen parameters
const int SCREEN_WIDTH  = 800 , // size of screen
          SCREEN_HEIGHT = 600 ,
          MAX_COLORS    = 256 ; // maximum colors (8 bits per pixel)

class Blackbox
{
 public:

   LPDIRECTDRAW4         lpdd         ; // dd object
   LPDIRECTDRAWSURFACE4  lpddsprimary ; // dd primary surface
   LPDIRECTDRAWPALETTE   lpddpal      ; // a pointer to the created dd palette   
   LPDIRECTDRAWCLIPPER   lpddclipper  ; // dd clipper

   PALETTEENTRY          palette[ MAX_COLORS ]     ; // color palette
   PALETTEENTRY          save_palette[ MAX_COLORS ]; // used to save palettes
 
   DDSURFACEDESC2        ddsd    ; // a direct draw surface description struct      
   DDBLTFX               ddbltfx ; // used to fill
   DDSCAPS2              ddscaps ; // a direct draw surface capabilities struct
 
   HRESULT               ddrval  ;     // result back from dd calls
   DWORD                 clock_start ; // used for timing

   // these define the general clipping rectangle
   int min_clip_x,  max_clip_x,                  
       min_clip_y,  max_clip_y ;                  

   // constructor
   Blackbox( );

   // DirectDraw functions
   int DD_Init      ( HWND, int, int, int );
   int DD_Shutdown  ( );

   LPDIRECTDRAWCLIPPER DD_Attach_Clipper( LPDIRECTDRAWSURFACE4, int, LPRECT );
   int DD_Flip( );
   int DD_Fill_Surface( LPDIRECTDRAWSURFACE4, int );

   // general utility functions
   DWORD Start_Clock( );
   DWORD Get_Clock  ( );
   DWORD Wait_Clock ( DWORD );

   // graphics functions
   void definePalette( );

   int DD_Blt_Rect   ( int, int, int, int, int,
                       LPDIRECTDRAWSURFACE4 lpdds = lpddsback );

   int DD_copyBitmap ( UCHAR *bitmap,   // file with the image data
                       int cx, int cy,            // cell to scan image from
                       LPDIRECTDRAWSURFACE4 lpdds4 = lpddsback );

   int DD_bltBitmap  ( int x, int y,          // position to draw bitmap
                       int width, int height, // size of bitmap in pixels
                       UCHAR *bitmap, UCHAR *surface,
                       int surfacePitch );

   int DD_bltHappy   ( int x1, int y1, int x2, int y2, int color,
                       LPDIRECTDRAWSURFACE4 lpdds = lpddsback );

   int GDI_Draw_Text ( char *, int, int, int, 
                       LPDIRECTDRAWSURFACE4 lpdds = lpddsback );
};
// class Blackbox

#endif // FREAKPROJ_BLACKBOX_H
