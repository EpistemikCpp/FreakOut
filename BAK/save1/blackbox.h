// blackbox.h - Header file for DirectX functions

// watch for multiple inclusions
#ifndef FREAKPROJ_BLACKBOX_H
#define FREAKPROJ_BLACKBOX_H

#include <ddraw.h>  // NEED THIS FOR DIRECTDRAW VARIABLE TYPES
#include <io.h>     // for _lseek()
#include <time.h>   // for the time fxns in Load_Bitmap_File()

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

// screen parameters
const int SCREEN_WIDTH  = 800 ,    // size of screen
          SCREEN_HEIGHT = 600 ,
          MAX_COLORS    = 256 ,    // max palette colors (8 bits per pixel)
          BITMAP_ID     = 0x4D42 ; // universal id for a bitmap

// container structure for bitmaps .BMP file
typedef struct BITMAP_CONTAINER_STRUCT
{
   BITMAPFILEHEADER fileheader ;  // contains the bitmapfile header
   BITMAPINFOHEADER infoheader ;  // all the info including the palette
   PALETTEENTRY     palette[ MAX_COLORS ]; // store the palette here
   UCHAR            *buffer ; // pointer to the data
}
 BITMAP_CONTAINER, *BITMAP_CTN_PTR ;

//*****************************************************************************

class Blackbox
{
 protected:

   LPDIRECTDRAW4         lpdd         ; // dd object
   LPDIRECTDRAWSURFACE4  lpddsprimary ; // dd primary surface
   LPDIRECTDRAWSURFACE4  lpddsbkimg   ; // hold the ball image
   LPDIRECTDRAWPALETTE   lpddpal      ; // a pointer to the created dd palette   
   LPDIRECTDRAWCLIPPER   lpddclipper  ; // dd clipper

   PALETTEENTRY          palette[ MAX_COLORS ]     ; // color palette
   PALETTEENTRY          save_palette[ MAX_COLORS ]; // used to save palettes
 
   DDSURFACEDESC2        ddsd    ; // a direct draw surface description struct      
   DDBLTFX               ddbltfx ; // used to fill
   DDSCAPS2              ddscaps ; // a direct draw surface capabilities struct
 
   HRESULT               ddrval  ;     // result back from dd calls
   DWORD                 clock_start ; // used for timing

   BITMAP_CONTAINER      ball_bm ;

   // these define the general clipping rectangle
   int min_clip_x,  max_clip_x,
       min_clip_y,  max_clip_y ;

   int bm_dd_lPitch ;

 public:

   // need a static if being used as a function default
   static LPDIRECTDRAWSURFACE4 lpddsback ;  // dd back surface

   // constructor
   Blackbox( );

   // DirectDraw functions
   int DD_Init        ( HWND, char*, int, int, int );
   int DD_Shutdown    ( );
   int DD_Flip        ( );
   int DD_Fill_Surface( LPDIRECTDRAWSURFACE4, int );
   // THE BACK BUFFER: lpddsback, IS THE DEFAULT SURFACE
   int DD_Blt_Rect    ( int, int, int, int, int,
                        LPDIRECTDRAWSURFACE4 lpdds = lpddsback );
   int DD_Draw_Surface( int, int, int transparent = 1 );
   int DD_copyBitmap  ( BITMAP_CTN_PTR, LPDIRECTDRAWSURFACE4, int, int );
   LPDIRECTDRAWSURFACE4 DD_Create_Surface( int, int, int, int color_key = 0 );
   LPDIRECTDRAWCLIPPER  DD_Attach_Clipper( LPDIRECTDRAWSURFACE4, int, LPRECT );

   // graphics functions
   void definePalette    ( );
   int Bitmap_Flip       ( UCHAR*, int, int );
   int Bitmap_Load_File  ( BITMAP_CTN_PTR, char* );
   int Bitmap_Unload_File( BITMAP_CTN_PTR );
   int GDI_Draw_Text     ( char*, int, int, int, 
                           LPDIRECTDRAWSURFACE4 lpdds = lpddsback );
   // timer functions
   DWORD Start_Clock( );
   DWORD Get_Clock  ( );
   DWORD Wait_Clock ( DWORD );
};
// class Blackbox

#endif // FREAKPROJ_BLACKBOX_H
