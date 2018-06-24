// bitmapFxns.h
// -- from DEMO7_13.CPP

// watch for multiple inclusions
#ifndef FREAKPROJ_BITMAPFXNS_H
#define FREAKPROJ_BITMAPFXNS_H

// INCLUDES ///////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  // just say no to MFC
#define INITGUID

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream>   // include important C/C++ stuff
#include <io.h>       // for _lseek()
#include <time.h>     // for the time fxns in Load_Bitmap_File()

#include <ddraw.h>    // include directdraw

///// TYPES //////////////////////////////////////////////////////

const int COLORS_8BIT_PALETTE = 256 ;

// basic unsigned types
typedef unsigned short USHORT ;
typedef unsigned short WORD   ;
typedef unsigned char  UCHAR  ;
typedef unsigned char  BYTE   ;

// container structure for bitmaps .BMP file
typedef struct BITMAP_CONTAINER_STRUCT
{
   BITMAPFILEHEADER bitmapfileheader ;  // contains the bitmapfile header
   BITMAPINFOHEADER bitmapinfoheader ;  // all the info including the palette
   PALETTEENTRY     palette[ COLORS_8BIT_PALETTE ]; // store the palette here
   UCHAR            *buffer ; // pointer to the data
}
 BITMAP_CONTAINER, *BITMAP_CTN_PTR ;

///// PROTOTYPES  ///////////////////////////////////////////////////

extern int errno ;

int Flip_Bitmap( UCHAR*, int, int );

int Load_Bitmap_File( BITMAP_CTN_PTR, char* );

int Unload_Bitmap_File( BITMAP_CTN_PTR );

int DD_Fill_Surface( LPDIRECTDRAWSURFACE4, int );

int copyBitmapToSurface( BITMAP_CTN_PTR, LPDIRECTDRAWSURFACE4, int, int );

LPDIRECTDRAWSURFACE4 DD_Create_Surface( int, int, int, int );

int DD_Draw_Surface( LPDIRECTDRAWSURFACE4, int, int, int, int,
                     LPDIRECTDRAWSURFACE4, int );    

LPDIRECTDRAWCLIPPER DD_Attach_Clipper( LPDIRECTDRAWSURFACE4, int, LPRECT );

int Draw_Text_GDI( char*, int, int, COLORREF, LPDIRECTDRAWSURFACE4 );

///// MACROS //////////////////////////////////////////////////////

// tests if a key is up or down
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// initializes a direct draw struct
#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); \
                                      ddstruct.dwSize=sizeof(ddstruct); }

///// GLOBALS /////////////////////////////////////////////////////

// bitmap parameters
int SCREEN_WIDTH  = 640 ; // size of screen
int SCREEN_HEIGHT = 480 ;
int SCREEN_BPP    =   8 ; // bits per pixel

int BITMAP_ID     = 0x4D42 ; // universal id for a bitmap

int MAX_COLORS_PALETTE = COLORS_8BIT_PALETTE ;

HWND main_window_handle = 0 ;     // globally track main window
bool window_closed      = false ; // tracks if window is closed

// directdraw stuff
LPDIRECTDRAW4         lpdd4        = 0 ; // dd4 object
LPDIRECTDRAWSURFACE4  lpddsprimary = 0 ; // dd primary surface
LPDIRECTDRAWSURFACE4  lpddsback    = 0 ; // dd back surface
LPDIRECTDRAWSURFACE4  lpddsbkimg   = 0 ; // hold the background image

DDSURFACEDESC2        ddsd ;          // direct draw surface description struct
LPDIRECTDRAWCLIPPER   lpddclipper = 0 ; // dd clipper

PALETTEENTRY          palette[ COLORS_8BIT_PALETTE ]; // color palette
LPDIRECTDRAWPALETTE   lpddpal = 0  ; // created dd palette

BITMAP_CONTAINER      bitmap    ;  // holds the bitmap
ALIEN_OBJ             aliens[3] ;  // 3 aliens, one on each level

int fg_bm_height = 80, fg_bm_width  = 72 ;

int bg_lPitch = 0, fg_lPitch = 0 ;

int rand_mod = 5 ;

int PAUSE_C = 50 ;

