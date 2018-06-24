/**************************************************************************
 *
 *  Mark Sattolo (msattolo@sympatico.ca)
 * -----------------------------------------------
 *  $File: //depot/CppProj/VS6/FreakProj/Src/blackbox.h $
 *  $Revision: #5 $
 *  $Change: 82 $
 *  $DateTime: 2008/12/29 10:02:24 $
 *
 * ---------------------------------------------------------------------
 *  blackbox.h - Header file for DirectX functions - CLASS Blackbox
 *		Created 2001, based on code in 
 *		'Tricks of the Windows Game Programming Gurus', by Andre Lamothe
 * 
 **************************************************************************/

// watch for multiple inclusions
#ifndef MHS_VS6_FREAKPROJ_BLACKBOX_H
#define MHS_VS6_FREAKPROJ_BLACKBOX_H

#include <ddraw.h> // NEED THIS FOR DIRECTDRAW VARIABLE TYPES
#include <io.h>    // for _lseek()
#include <time.h>  // for the time fxns in Bitmap_Load_File()

// initialize a direct draw struct
#define DD_INIT_STRUCT(ddstruct) { memset( &ddstruct, 0, sizeof(ddstruct) ); \
                                   ddstruct.dwSize = sizeof(ddstruct); }

// color indices - see definePalette8()
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

// container for bitmaps
typedef struct BITMAP_CONTAINER_STRUCT
{
  BITMAPFILEHEADER  fileheader ;           // contains the bitmapfile header
  BITMAPINFOHEADER  infoheader ;           // all the info including the palette
  PALETTEENTRY      palette[ MAX_COLORS ]; // store the palette here
  UCHAR            *buffer ;               // pointer to the data
}
 BITMAP_CONTAINER, *pBITMAP_CONTAINER ;

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
 
   DDSURFACEDESC2        ddsd    ; // a direct draw surface description struct      
   DDBLTFX               ddbltfx ; // used to fill
 
   BITMAP_CONTAINER      ball_bm ;

   // these define the general clipping rectangle
   int min_clip_x,  max_clip_x,
       min_clip_y,  max_clip_y ;

 public:

   // need a static if being used as a function default
   static LPDIRECTDRAWSURFACE4 lpddsback ;  // dd back surface

// FUNCTIONS 

   Blackbox( ); // CONSTRUCTOR

  /* DirectDraw functions  */

   bool DD_Init        ( HWND, char*, int, int, int );
   void DD_Shutdown    ( );
   void DD_Flip        ( );

   // THE BACK BUFFER: lpddsback is the default surface
   bool DD_Blt_Rect    ( int, int, int, int, int,
                         LPDIRECTDRAWSURFACE4 lpdds = lpddsback );
   bool DD_Draw_Surface( int, int, int transparent = 1 );
 protected:
   bool DD_Fill_Surface( LPDIRECTDRAWSURFACE4, int );
   void DD_copyBitmap  ( pBITMAP_CONTAINER, LPDIRECTDRAWSURFACE4, int, int );

   LPDIRECTDRAWSURFACE4 DD_Create_Surface( int, int, int, int color_key = 0 );
   LPDIRECTDRAWCLIPPER  DD_Attach_Clipper( LPDIRECTDRAWSURFACE4, int, LPRECT );

   // graphics functions
   void definePalette8    ( );
   bool Bitmap_Flip       ( UCHAR*, int, int );
   bool Bitmap_Load_File  ( pBITMAP_CONTAINER, char* );
   void Bitmap_Unload_File( pBITMAP_CONTAINER );
 public:
   bool GDI_Draw_Text     ( char*, int, int, int, 
                            LPDIRECTDRAWSURFACE4 lpdds = lpddsback );
}; // CLASS Blackbox

#endif // MHS_VS6_FREAKPROJ_BLACKBOX_H
