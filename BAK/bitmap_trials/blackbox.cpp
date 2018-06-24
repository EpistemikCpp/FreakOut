// BLACKBOX.CPP - DirectX functions
 
#define INITGUID  // NEED THIS FOR LINKER TO FIND SYMBOL 'IID_IDIRECTDRAW4'

#include <windows.h>    // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>

#include <iostream>     // include important C/C++ stuff

#include "blackbox.h"   // game library includes
                                   
/* NEED A GLOBAL LPDIRECTDRAWSURFACE4 TO USE AS THE DEFAULT PARAMETER IN THE 
   DIRECT_DRAW FUNCTIONS - THIS PARAMETER NEEDS TO BE DECLARED IN BLACKBOX.CPP
   AND REFERENCED AS AN EXTERN IN BLACKBOX.H
   -> FROM BLACKBOX.H IT IS VISIBLE IN FREAKOUT.CPP & FREAKMAIN.CPP */
  LPDIRECTDRAWSURFACE4  lpddsback ;    // dd back surface

  // a low tech bitmap that uses palette entry 86 (YELLOW) for the color :)
  UCHAR happy_bitmap[64] = { 0,  0,  0,  0,  0,  0,  0, 0,
                             0,  0, 86, 86, 86, 86,  0, 0,
                             0, 86,  0, 86, 86,  0, 86, 0,
                             0, 86, 86, 86, 86, 86, 86, 0,
                             0, 86,  0, 86, 86,  0, 86, 0,
                             0, 86, 86,  0,  0, 86, 86, 0,
                             0,  0, 86, 86, 86, 86,  0, 0,
                             0,  0,  0,  0,  0,  0,  0, 0 } ;

///// FUNCTION DEFINITIONS /////////////////////////////////////////////

// CONSTRUCTOR - INITIALIZE OBJECTS
Blackbox::Blackbox()
{
   lpdd         = NULL ;   // dd object
   lpddsprimary = NULL ;   // dd primary surface
   lpddsback    = NULL ;   // dd back surface
   lpddpal      = NULL ;   // a pointer to the created dd palette
   lpddclipper  = NULL ;   // dd clipper
   clock_start  = 0 ;      // used for timing

// these define the general clipping rectangle
   min_clip_x = 0 ;                       
   max_clip_x = SCREEN_WIDTH-1 ;
   min_clip_y = 0 ;
   max_clip_y = SCREEN_HEIGHT-1 ;
}

//=============================================================================

int Blackbox::DD_Init( HWND main_window_handle, int width, int height, int bpp )
{
   // THIS FUNCTION INITIALIZES DIRECTDRAW

   LPDIRECTDRAW lpdd1 = NULL ; // used to query for the latest interface

   // create object and test for error
   if( DirectDrawCreate(NULL, &lpdd1, NULL) != DD_OK )
     return 0 ;

   // query for the latest version of the interface (4)
   if( lpdd1->QueryInterface(IID_IDirectDraw4, (LPVOID*)&lpdd) != DD_OK ) 
     return 0 ;

   // set cooperation level to full screen exclusive mode
   if( lpdd->SetCooperativeLevel
           ( main_window_handle,
             DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT ) != DD_OK )
     return 0 ;

   // set the display mode
   if( lpdd->SetDisplayMode(width, height, bpp, 0, 0) != DD_OK )
     return 0 ;

   // Create the primary surface
   memset( &ddsd, 0, sizeof(ddsd) );
   ddsd.dwSize = sizeof ddsd ;
   ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT ;

   // we need to let dd know that we want a complex 
   // flippable surface structure, set flags for that
   ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

   // set the backbuffer count to 1
   ddsd.dwBackBufferCount = 1 ;

   // create the primary surface
   lpdd->CreateSurface( &ddsd, &lpddsprimary, NULL );

   // query for the backbuffer i.e the secondary surface
   ddscaps.dwCaps = DDSCAPS_BACKBUFFER ;
   lpddsprimary->GetAttachedSurface( &ddscaps, &lpddsback );

   // CREATE AND ATTACH PALETTE
   // define palette data
   definePalette();

   // now create the palette object
   if( lpdd->CreatePalette( DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256,
		                      palette, &lpddpal, NULL ) != DD_OK  )
     return 0 ;

   // attach the palette to the primary
   if( lpddsprimary->SetPalette(lpddpal) != DD_OK )
     return 0 ;

   // clear out both primary and secondary surfaces
   DD_Fill_Surface( lpddsprimary, 0 );
   DD_Fill_Surface( lpddsback,    0 );

   // attach a clipper to the screen
   RECT screen_rect = {0, 0, width, height} ;
   lpddclipper = DD_Attach_Clipper( lpddsback, 1, &screen_rect );

   // return success
   return TRUE ;

}// DD_Init()

//=============================================================================

int Blackbox::DD_Shutdown()
{
 // THIS FUNCTION RELEASES ALL THE RESOURCES DIRECTDRAW ALLOCATED,
 // MAINLY TO COM OBJECTS

 // release the clipper first
 if( lpddclipper ) lpddclipper->Release();

 // release the palette
 if( lpddpal ) lpddpal->Release();

 // release the secondary surface
 if( lpddsback ) lpddsback->Release();

 // release the primary surface
 if( lpddsprimary ) lpddsprimary->Release();

 // finally, the main dd object
 if( lpdd ) lpdd->Release();

 // return success
 return TRUE ;

}// DD_Shutdown()

//=============================================================================

// create a R,G,B palette
void Blackbox::definePalette()
{
   int myRed, myGreen, myBlue ;

   // set all values to zero first - defensive programming
   memset( palette, 0, 256 * sizeof(PALETTEENTRY) );

   // SET COLORS
   for( int index = 0; index < 256; index++ )
   {
      // a little brightness for each range
      myRed = myGreen = myBlue = 10 ; 

      if( index == 0 ) 
        myRed = myGreen = myBlue = 1 ; // black
      else if( index == 1 )
          myRed = myGreen = myBlue = 87 ; // grey
      else if( index == 2 )
          myRed = myGreen = myBlue = 254 ; // white
      else if( index == 3 )
          { myRed = 254 ;
            myGreen = 99 ;
            myBlue = 201 ; } // pink
      else if( index < 32 )
          myRed = 140 + (index-4)*4 ; // shades of red
      else if( index < 60 )
          { myRed = 140 + (index-32)*4 ; 
            myGreen = 60 + (index-32)*4 ; } // shades of orange
      else if( index < 88 )
          myRed = myGreen = 140 + (index-60)*4 ; // shades of yellow
      else if( index < 116 )
          myGreen = 140 + (index-88)*4 ; // shades of bright green
      else if( index < 144 )
          myGreen = 60 + (index-116)*4 ; // shades of dark green
      else if( index < 172 )
          myGreen = myBlue = 140 + (index-144)*4 ; // shades of cyan
      else if( index < 200 )
          myBlue = 140 + (index-172)*4 ; // shades of blue
      else if( index < 228 )
          { myRed = 60 + (index-200)*4 ; 
            myBlue = 140 + (index-200)*4 ; } // shades of violet
      else //( index < 256 )
          myRed = myBlue = 140 + (index-228)*4 ; // shades of magenta

      palette[index].peRed = myRed ;
      palette[index].peGreen = myGreen ;
		palette[index].peBlue = myBlue ;

      // set flags
      palette[index].peFlags = PC_NOCOLLAPSE ;
   } // end for(index)

}// define_palette()

//=============================================================================

LPDIRECTDRAWCLIPPER Blackbox::DD_Attach_Clipper( LPDIRECTDRAWSURFACE4 lpdds,
                                                 int num_rects,
                                                 LPRECT clip_list )
{
/* CREATE A CLIPPER FROM THE SENT CLIP LIST AND ATTACH IT TO THE SENT SURFACE */

 int index ;                       // looping var
 LPDIRECTDRAWCLIPPER lpddclipper ; // pointer to the newly created dd clipper
 LPRGNDATA region_data ;           /* pointer to the region data that contains
                                      the header and clip list   */

 // first create the direct draw clipper
 if( (lpdd->CreateClipper(0, &lpddclipper, NULL)) != DD_OK )
   return NULL ;

 // now create the clip list from the sent data

 // first allocate memory for region data
 region_data 
      = (LPRGNDATA)malloc( sizeof(RGNDATAHEADER) + num_rects*sizeof(RECT) );

 // now copy the rects into region data
 memcpy( region_data->Buffer, clip_list, sizeof(RECT)*num_rects );

 // set up fields of header
 region_data->rdh.dwSize          = sizeof( RGNDATAHEADER );
 region_data->rdh.iType           = RDH_RECTANGLES ;
 region_data->rdh.nCount          = num_rects ;
 region_data->rdh.nRgnSize        = num_rects * sizeof( RECT );

 region_data->rdh.rcBound.left    =  64000 ;
 region_data->rdh.rcBound.top     =  64000 ;
 region_data->rdh.rcBound.right   = -64000 ;
 region_data->rdh.rcBound.bottom  = -64000 ;

 // find bounds of all clipping regions
 for( index=0; index < num_rects; index++ )
 {
    // test if the next rectangle unioned with the current bound is larger
    if( clip_list[index].left < region_data->rdh.rcBound.left )
      region_data->rdh.rcBound.left = clip_list[index].left ;

    if( clip_list[index].right > region_data->rdh.rcBound.right )
      region_data->rdh.rcBound.right = clip_list[index].right ;

    if( clip_list[index].top < region_data->rdh.rcBound.top )
      region_data->rdh.rcBound.top = clip_list[index].top ;

    if( clip_list[index].bottom > region_data->rdh.rcBound.bottom )
      region_data->rdh.rcBound.bottom = clip_list[index].bottom ;
 }
 /* WE HAVE COMPUTED THE BOUNDING RECTANGLE REGION AND SET UP THE DATA
    - NOW LET'S SET THE CLIPPING LIST */

 if( (lpddclipper->SetClipList(region_data, 0)) != DD_OK )
 {
   free( region_data ); // release memory and return error
   return NULL ;
 }

 // now attach the clipper to the surface
 if( (lpdds->SetClipper(lpddclipper)) != DD_OK )
 {
   free( region_data ); // release memory and return error
   return NULL ;
 }

 // all is well, so release memory and send back the pointer to the new clipper
 free( region_data );
 return lpddclipper ;

}// DD_Attach_Clipper()

//=============================================================================

int Blackbox::DD_Flip()
{
 // THIS FUNCTION FLIPS THE PRIMARY SURFACE WITH THE SECONDARY SURFACE

 // flip pages
 while( lpddsprimary->Flip(NULL, DDFLIP_WAIT) != DD_OK )
   ;// keep trying to flip while FAILED

 // return success
 return TRUE ;

}// DD_Flip()

//=============================================================================

DWORD Blackbox::Start_Clock()
{
 // THIS FUNCTION STARTS THE CLOCK, THAT IS, SAVES THE CURRENT COUNT,
 // use in conjunction with Wait_Clock()
 return( clock_start = Get_Clock() );

}// Start_Clock()

//=============================================================================

DWORD Blackbox::Get_Clock()
{
 // RETURN THE CURRENT TICK COUNT
 return GetTickCount();

}// Get_Clock()

//=============================================================================

DWORD Blackbox::Wait_Clock( DWORD count )
{
 // WAIT FOR A SPECIFIC NUMBER OF CLICKS SINCE THE CALL TO START_CLOCK
 while( (Get_Clock() - clock_start) < count )
	; // wait
 
 return Get_Clock();

}// Wait_Clock()

//=============================================================================

int Blackbox::DD_Fill_Surface( LPDIRECTDRAWSURFACE4 lpdds, int color )
{
 DDBLTFX ddbltfx ;

 // clear out the structure and set the size field 
 DD_INIT_STRUCT( ddbltfx );

 // set the dwfillcolor field to the desired color
 ddbltfx.dwFillColor = color ; 

 // ready to blt to surface
 lpdds->Blt( NULL,        // ptr to dest rectangle
             NULL,        // ptr to source surface, NA            
             NULL,        // ptr to source rectangle, NA
             DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC,   // fill and wait                   
             &ddbltfx );  // ptr to DDBLTFX structure

 // return success
 return TRUE ;

}// DD_Fill_Surface()

//=============================================================================

int Blackbox::DD_Blt_Rect( int x1, int y1, int x2, int y2, int color,
                           LPDIRECTDRAWSURFACE4 lpdds )
{
 // THIS FUNCTION USES DIRECTDRAW TO DRAW A FILLED RECTANGLE

 DDBLTFX ddbltfx ; // this contains the DDBLTFX structure
 RECT fill_area ;  // this contains the destination rectangle

 // clear out the structure and set the size field 
 DD_INIT_STRUCT( ddbltfx );

 // set the dwfillcolor field to the desired color
 ddbltfx.dwFillColor = color ; 

 // fill in the destination rectangle data (your data)
 fill_area.top    = y1 ;
 fill_area.left   = x1 ;
 fill_area.bottom = y2+1 ;
 fill_area.right  = x2+1 ;

 // ready to blt to surface
 lpdds->Blt( &fill_area,  // ptr to dest rectangle
             NULL,        // ptr to source surface, NA            
             NULL,        // ptr to source rectangle, NA
             DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC,  // fill and wait                   
             &ddbltfx );  // ptr to DDBLTFX structure

 // return success
 return TRUE ;

}// DD_Blt_Rect()

//=============================================================================

int Blackbox::DD_bltHappy( int x1, int y1, int x2, int y2, int color,
                            LPDIRECTDRAWSURFACE4 lpdds )
{
 // THIS FUNCTION USES DIRECTDRAW TO DRAW happy_bitmap
/*
 DDBLTFX ddbltfx ;
 RECT fill_area ; // the destination rectangle

 RECT* source_rect = (RECT*)happy_bitmap ;
 
 // clear out the structure and set the size field 
 DD_INIT_STRUCT( ddbltfx );

 // set the dwfillcolor field to the desired color
 ddbltfx.dwFillColor = color ; 

 // fill in the destination rectangle data (your data)
 fill_area.top    = y1 ;
 fill_area.left   = x1 ;
 fill_area.bottom = y2+1 ;
 fill_area.right  = x2+1 ;

 // ready to blt to surface, in this case blt to primary
 lpdds->Blt( &fill_area,  // ptr to dest rectangle
             NULL,        // ptr to source surface, NA            
             source_rect,        // ptr to source rectangle
             DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC,  // fill and wait                   
             &ddbltfx );  // ptr to DDBLTFX structure

 // return success
 return TRUE ;
*/

// ORIGINAL FROM DEMO7.8

DDBLTFX ddbltfx; // the blitter fx structure

// USE THE BLITTER TO ERASE THE BACK BUFFER

// first initialize the DDBLTFX structure
DD_INIT_STRUCT(ddbltfx);

// now set the color word info to the color we desire
ddbltfx.dwFillColor = 0 ;

// make the blitter call
if( FAILED(lpddsback->Blt( NULL, // pointer to dest RECT, NULL for whole thing
                           NULL, // pointer to source surface
                           NULL, // pointer to source RECT, NULL for whole thing
                           DDBLT_COLORFILL | DDBLT_WAIT, 
                           // do a color fill and wait if you have to
                           &ddbltfx )) // pointer to DDBLTFX holding info
  )return 0 ;

// initialize ddsd
DD_INIT_STRUCT(ddsd);

// lock the back buffer surface
if( FAILED(lpddsback->Lock( NULL, &ddsd, 
                            DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL )) )
  return 0 ;
/*
   Blit_Clipped( happy_faces[face].x, 
                 happy_faces[face].y,
                 8, 8,    
                 happy_bitmap, 
                 (UCHAR *)ddsd.lpSurface,
                 ddsd.lPitch );         

void Blit_Clipped(
int x = happy_bitmap, int y,          // position to draw bitmap
             //e.g.  400,  -100 -> SEE BOOK FOR DIAGRAM
                   int width, int height, // size of bitmap in pixels
             //e.g.      320,        200
                   UCHAR *bitmap,         // pointer to bitmap data
                   UCHAR *video_buffer,   // pointer to video buffer surface
                   int mempitch )         // video pitch per line
*/
// this function blits and clips the image sent in bitmap to the 
// destination surface pointed to by video_buffer
// the function assumes a 640x480x8 mode 
// i.e. SCREEN_WIDTH == 640, SCREEN_HEIGHT == 480
// this function is slightly different than the one in the book
// ie, it doesn't assume linear pitch 

/*
// first do trivial rejections of bitmap, is it totally invisible?
if( (x >= SCREEN_WIDTH) || (y>= SCREEN_HEIGHT)
    || ((x + width) <= 0) || ((y + height) <= 0) )
  return ;

// CLIP SOURCE RECTANGLE
// pre-compute the bounding rect to make life easy
int x1 = x ;               // x1 ==  400
int y1 = y ;               // y1 == -100
int x2 = x1 + width - 1 ;  // x2 ==  719
int y2 = y1 + height -1 ;  // y2 ==   99

// upper left hand corner first
if( x1 < 0 ) x1 = 0 ;      // x1 == 400

if( y1 < 0 ) y1 = 0 ;      // y1 ==   0

// now lower right hand corner
if( x2 >= SCREEN_WIDTH )
  x2 = SCREEN_WIDTH-1 ;    // x2 == 639

if( y2 >= SCREEN_HEIGHT )
  y2 = SCREEN_HEIGHT-1 ;   // y2 ==  99


// now we know to draw only the portions of the bitmap from (x1,y1)
// to (x2,y2) compute offsets into bitmap on x,y axes, we need this 
// to compute starting point to rasterize from
int x_off = x1 - x ;       // x_off ==   0
int y_off = y1 - y ;       // y_off == 100

// COMPUTE STARTING ADDRESS IN BITMAP TO SCAN DATA FROM
bitmap += (x_off + y_off*width) ;    // bitmap == [0 + 100*320]

// COMPUTE STARTING ADDRESS IN VIDEO_BUFFER 
video_buffer += (x1 + y1*mempitch) ; // video_buffer == [400 + 0]

// compute number of columns and rows to blit
int dx = x2 - x1 + 1 ;     // dx == 240
int dy = y2 - y1 + 1 ;     // dy == 100

// at this point bitmap is pointing to the first pixel in the bitmap 
// that needs to be blitted, and video_buffer is pointing to the 
// memory location on the destination buffer to put it, 
// so now enter rasterizer loop

UCHAR pixel ; // used to read/write pixels

for( int index_y = 0; index_y < dy; index_y++ ) // dy == 100
  {
   // inner loop, where the action takes place
   for( int index_x = 0; index_x < dx; index_x++ ) // dx == 240
     {
      // read pixel from source bitmap, test for transparency and plot
      if( pixel = bitmap[index_x] )
        video_buffer[index_x] = pixel ;
     }// end for index_x
     
   // advance pointers
   video_buffer += mempitch ;  // bytes per scanline
   bitmap += width ;           // bytes per bitmap row

  }// end for index_y

// unlock surface
if( FAILED(lpddsback->Unlock(NULL)) )
  return 0 ;

// flip the pages
while( FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)) )
  ; // do nothing

*/

}// DD_bltHappy()

//=============================================================================

int Blackbox::DD_bltBitmap( int x, int y,          // position to draw bitmap
                            int width, int height, // size of bitmap in pixels
                            UCHAR *bitmap, UCHAR *surface,
                            int surfacePitch )
{
/* FROM Blit_Clipped() in Demo 7.8
   this function blits and clips the image sent in bitmap to the 
   destination surface pointed to by video_buffer
   the function assumes a 640x480x8 mode 
   this function is slightly different than the one in the book
   ie, it doesn't assume linear pitch 
*/
// UCHAR *bitmap = happy_bitmap ; // pointer to bitmap data

   // first do trivial rejections of bitmap, is it totally invisible?
 if( (x >= SCREEN_WIDTH) || (y >= SCREEN_HEIGHT)
      || ((x + width) <= 0) || ((y + height) <= 0) )
   return false ;

 // clip source rectangle
 // pre-compute the bounding rect to make life easy
 int x1 = x,
     y1 = y ;
 int x2 = ( x1 +  width - 1 ),
     y2 = ( y1 + height - 1 );

 // upper left hand corner first
 if( x1 < 0 ) x1 = 0 ;
 if( y1 < 0 ) y1 = 0 ;

 // now lower right hand corner
 if( x2 >= SCREEN_WIDTH  ) x2 =  SCREEN_WIDTH - 1 ;
 if( y2 >= SCREEN_HEIGHT ) y2 = SCREEN_HEIGHT - 1 ;
/*
 // now we know to draw only the portions of the bitmap from (x1,y1) to (x2,y2)
 // compute offsets into bitmap on x,y axes, we need this to compute 
 // starting point to rasterize from
 int x_off = x1 - x,
     y_off = y1 - y ;

 // compute number of columns and rows to blit
 int dx = x2 - x1 + 1,
     dy = y2 - y1 + 1 ;
*/
 // at this point bitmap is pointing to the first pixel in the bitmap that
 // needs to be blitted, and video_buffer is pointing to the memory location
 // on the destination buffer to put it, so now enter rasterizer loop

 // compute starting address in video_buffer 
 surface += ( x1 + y1*surfacePitch );

 // compute starting address in bitmap to scan data from
// bitmap += ( x_off + y_off*width );

// iterate thru each scanline and copy bitmap
for( int index_y=0; index_y < (int)height; index_y++ )
{
    // copy next line of data to destination
    memcpy( surface, bitmap, width );

    // advance pointers
    surface += surfacePitch ;//!! TAKE THE SURFACE DIMENSION INTO ACCOUNT !!
    bitmap += width ;

} // end for index_y
/*
 UCHAR pixel ; // used to read/write pixels
 for( int index_y = 0; index_y < height; index_y++ )
 {
   // inner loop, where the action takes place
   for( int index_x = 0; index_x < width; index_x++ )
   {
     // read pixel from source bitmap, test for transparency and plot
     if( pixel = bitmap[index_x] )
       video_buffer[index_x] = pixel ;
   }
     
   // advance pointers
   video_buffer += mempitch ;  // bytes per scanline
   bitmap       += width ;     // bytes per bitmap row

 }
*/
 // return success
 return TRUE ;

}// DD_bltBitmap()

//=============================================================================

int Blackbox::DD_copyBitmap( UCHAR *bitmap,   // file with the image data
                             int cx, int cy,            // cell to scan image from
                             LPDIRECTDRAWSURFACE4 lpdds4 ) // surface to hold data
{
// COPY A BITMAP FROM A BITMAP_CONTAINER TO A DD SURFACE

UCHAR *source_ptr, *dest_ptr ; // working pointers

DDSURFACEDESC2 ddsd_temp ;  //  direct draw surface description 

// GET THE ADDR TO DESTINATION SURFACE MEMORY

// set size of the structure
ddsd_temp.dwSize = sizeof ddsd_temp ;
/*
  // set dimensions of the new bitmap surface
  ddsd_temp.dwWidth  = fg_bm_width  ; // 72
  ddsd_temp.dwHeight = fg_bm_height ; // 80
*/
// lock the display surface
lpdds4->Lock( 0, &ddsd_temp, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0 );

  // for the GDI display in GameMain()
  // -- SURFACE MUST BE LOCKED TO BE READ FROM PROPERLY
//  fg_lPitch = ddsd_temp.lPitch ;

// compute position to start scanning bits from
// EACH CELL OF THE TEMPLATED BITMAP HAS A 1-BIT BORDER SURROUNDING IT
cx = cx*(  ddsd_temp.dwWidth + 1 ) + 1 ;
cy = cy*( ddsd_temp.dwHeight + 1 ) + 1 ;

// extract bitmap data
source_ptr = bitmap;//->buffer + (cy*bitmap->bitmapinfoheader.biWidth) + cx ;

// assign a pointer to the memory surface for manipulation
dest_ptr = (UCHAR*)ddsd_temp.lpSurface ;

// iterate thru each scanline and copy bitmap
for( int index_y=0; index_y < (int)ddsd_temp.dwHeight; index_y++ )
{
    // copy next line of data to destination
    memcpy( dest_ptr, source_ptr, ddsd_temp.dwWidth );

    // advance pointers
    dest_ptr   += ddsd_temp.lPitch ;//!! TAKE THE SURFACE DIMENSION INTO ACCOUNT !!
//    source_ptr += bitmap->bitmapinfoheader.biWidth ;

} // end for index_y

// unlock the surface 
lpdds4->Unlock( 0 );

// return success
return 1 ;

} // end copyBitmapToSurface

///////////////////////////////////////////////////////////////

int Blackbox::GDI_Draw_Text( char *text, int x, int y, int color,
									  LPDIRECTDRAWSURFACE4 lpdds )
{
 // DRAW THE SENT TEXT ON THE SENT SURFACE USING THE COLOR IN THE PALETTE

 HDC xdc ; // the working dc

 // get the dc from surface
 if( lpdds->GetDC(&xdc) != DD_OK )
   return 0 ;

 // set the colors for the text up
 SetTextColor( xdc, 
               RGB( palette[color].peRed, 
                    palette[color].peGreen, palette[color].peBlue )  );

 // set background mode to transparent so black isn't copied
 SetBkMode( xdc, TRANSPARENT );

 // draw the text
 TextOut( xdc, x, y, text, strlen(text) );

 // release the dc
 lpdds->ReleaseDC( xdc );

 // return success
 return TRUE ;
}
// GDI_Draw_Text()
