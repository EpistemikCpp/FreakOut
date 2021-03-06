// blackbox.cpp
// - DirectX and Utility functions
 
#define INITGUID  // NEED THIS FOR LINKER TO FIND SYMBOL 'IID_IDIRECTDRAW4'

#include <windows.h>    // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream>     // include important C/C++ stuff

#include "blackbox.h"   // game library includes
                                   
// set in DD_Init() with the value from the bitmap
int BALL_SIZE ;

/******************************************************************************
       FUNCTION DEFINITIONS 
*******************************************************************************/

// CONSTRUCTOR - initialize member variables
Blackbox::Blackbox()
{
  lpdd         = NULL ;  // dd object
  lpddsprimary = NULL ;  // dd primary surface
  lpddsbkimg   = NULL ;  // hold the ball image
  lpddpal      = NULL ;  // a pointer to the created dd palette
  lpddclipper  = NULL ;  // dd clipper
  clock_start  = 0 ;     // used for timing

// these define the general clipping rectangle
  min_clip_x = 0 ;                       
  max_clip_x = SCREEN_WIDTH-1 ;
  min_clip_y = 0 ;
  max_clip_y = SCREEN_HEIGHT-1 ;
}

//=============================================================================

// INITIALIZES DIRECTDRAW
int Blackbox::DD_Init( HWND main_window, char *ball_name, 
                       int width, int height, int bpp     )
{
  LPDIRECTDRAW lpdd1 = NULL ; // used to query for the latest interface

  // create object and test for error
  if( DirectDrawCreate(NULL, &lpdd1, NULL) != DD_OK )
    return 0 ;

  // query for the latest version of the interface (4)
  if( lpdd1->QueryInterface(IID_IDirectDraw4, (LPVOID*)&lpdd) != DD_OK ) 
    return 0 ;

  // set cooperation level to full screen exclusive mode
  if( lpdd->SetCooperativeLevel
           ( main_window,
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

  // attach a clipper to the back buffer
  RECT screen_rect = { 0, 0, width, height };
  lpddclipper = DD_Attach_Clipper( lpddsback, 1, &screen_rect );

  // LOAD THE BITMAP WITH THE BALL IMAGE & COPY THE IMAGE INTO THE DD SURFACE

  // load the 8-bit moving image
  if( !Bitmap_Load_File(&ball_bm, ball_name) )
    return 0 ;

  // create surface to hold image
  lpddsbkimg = DD_Create_Surface( ball_bm.infoheader.biWidth,
                                   ball_bm.infoheader.biHeight, 0 );

  // now load bits...
  DD_copyBitmap( &ball_bm, lpddsbkimg,
                 ball_bm.infoheader.biWidth, ball_bm.infoheader.biHeight );  

  // unload the bitmap file, we no longer need it
  Bitmap_Unload_File( &ball_bm );

  // get the dimensions of the ball
  BALL_SIZE = ball_bm.infoheader.biWidth ; // assume a square bitmap

  // return success
  return TRUE ;

}// DD_Init()

//=============================================================================

// RELEASE ALL THE RESOURCES DIRECTDRAW ALLOCATED, MAINLY TO COM OBJECTS
int Blackbox::DD_Shutdown()
{
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

// CREATE A CLIPPER FROM THE SENT CLIP LIST AND ATTACH IT TO THE SENT SURFACE
LPDIRECTDRAWCLIPPER Blackbox::DD_Attach_Clipper( LPDIRECTDRAWSURFACE4 lpdds,
                                                 int num_rects,
                                                 LPRECT clip_list )
{
  int index ;                       // looping var
  LPDIRECTDRAWCLIPPER lpddclipper ; // pointer to the newly created dd clipper
  LPRGNDATA region_data ;           /* pointer to the region data that contains
                                      the header and clip list   */

  // first create the direct draw clipper
  if( (lpdd->CreateClipper(0, &lpddclipper, NULL)) != DD_OK )
    return NULL ;

  // NOW CREATE THE CLIP LIST FROM THE SENT DATA

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

// WE HAVE COMPUTED THE BOUNDING RECTANGLE REGION AND SET UP THE DATA
// - NOW LET'S SET THE CLIPPING LIST

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

// FLIP THE PRIMARY SURFACE WITH THE SECONDARY SURFACE
int Blackbox::DD_Flip()
{
  // flip pages
  while( lpddsprimary->Flip(NULL, DDFLIP_WAIT) != DD_OK )
    ;// keep trying to flip while FAILED

  // return success
  return TRUE ;

}// DD_Flip()

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

// USE DIRECTDRAW TO DRAW A FILLED RECTANGLE
int Blackbox::DD_Blt_Rect( int x1, int y1, int x2, int y2, int color,
                           LPDIRECTDRAWSURFACE4 lpdds )
{
  DDBLTFX ddbltfx ; // this contains the DDBLTFX structure
  RECT fill_area  ; // this contains the destination rectangle

  // clear out the structure and set the size field 
  DD_INIT_STRUCT( ddbltfx );

  // set the dwfillcolor field to the desired color
  ddbltfx.dwFillColor = color ; 

  // fill in the destination rectangle data (your data)
  fill_area.top    = y1   ;
  fill_area.left   = x1   ;
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

// CREATE AN OFFSCREEN PLAIN SURFACE
LPDIRECTDRAWSURFACE4 Blackbox::DD_Create_Surface( int width, int height,
                                                  int mem_flags, int color_key )
{
 DDSURFACEDESC2 ddsd ;         // working description
 LPDIRECTDRAWSURFACE4 lpdds ;  // temporary surface
    
 // set to access caps, width, and height
 memset( &ddsd, 0, sizeof(ddsd) );
 ddsd.dwSize  = sizeof( ddsd );
 ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CKSRCBLT ;

 // set dimensions of the new bitmap surface
 ddsd.dwWidth  = width  ;
 ddsd.dwHeight = height ;

 // set surface to offscreen plain
 ddsd.ddsCaps.dwCaps = // DDSCAPS_OFFSCREENPLAIN
                       DDSCAPS_VIDEOMEMORY // SHOULD BE FASTER
                       | mem_flags ;

 // create the surface
 if( FAILED(lpdd->CreateSurface(&ddsd, &lpdds, 0)) )
   return 0 ;

 // test if user wants a color key
 if( color_key >= 0 )
 {
   // set color key to color 0
   DDCOLORKEY color_key ; // used to set color key
   color_key.dwColorSpaceLowValue  = 0 ;
   color_key.dwColorSpaceHighValue = 0 ;

   // now set the color key for source blitting
   lpdds->SetColorKey( DDCKEY_SRCBLT, &color_key );
 }

 // return surface
 return( lpdds );

}// DD_Create_Surface()

//=============================================================================

int Blackbox::DD_Draw_Surface( int x, int y,     // position to draw at
                               int transparent ) // transparency flag
{
  // JUST DOES THE BALL BITMAP BY DEFAULT FOR NOW

  RECT dest_rect,    // the destination rectangle
       source_rect ; // the source rectangle                             

  int width  = ball_bm.infoheader.biWidth ;
  int height = ball_bm.infoheader.biHeight ;

  // fill in the destination rect
  dest_rect.left   = x ;
  dest_rect.top    = y ;
  dest_rect.right  = x + width - 1  ;
  dest_rect.bottom = y + height - 1 ;

  // fill in the source rect
  source_rect.left    = 0 ;
  source_rect.top     = 0 ;
  source_rect.right   = width - 1  ;
  source_rect.bottom  = height - 1 ;

  // test transparency flag
  if( transparent )
  {
    // enable color key blit
    // blt to destination surface
    if( FAILED(lpddsback->Blt( &dest_rect, lpddsbkimg, &source_rect,
                               (DDBLT_WAIT | DDBLT_KEYSRC), 0 )) )
      return 0 ;
  }
  else
    {
      // perform blit without color key
      // blt to destination surface
      if( FAILED(lpddsback->Blt( &dest_rect, lpddsbkimg, &source_rect,
                                 (DDBLT_WAIT), 0 )) )
        return 0 ;
    }

  // return success
  return 1 ;

}// DD_Draw_Surface()

//=============================================================================

// COPY A BITMAP FROM A BITMAP_CONTAINER TO A DD SURFACE
int Blackbox::DD_copyBitmap( BITMAP_CTN_PTR bitmap, // struct with the image data
                             LPDIRECTDRAWSURFACE4 lpdds4, // surface to hold data
                             int width, int height ) // dimensions of image
{
  UCHAR *source_ptr, *dest_ptr ; // working pointers

  DDSURFACEDESC2 ddsd_temp ;  //  direct draw surface description 

  // GET THE ADDR TO DESTINATION SURFACE MEMORY

  // set size of the structure
  ddsd_temp.dwSize = sizeof ddsd_temp ;

  // set dimensions of the new bitmap surface
  ddsd_temp.dwWidth  = width  ; // 72
  ddsd_temp.dwHeight = height ; // 80

  // lock the display surface
  lpdds4->Lock( 0, &ddsd_temp, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0 );

  // display this value in GameMain() using GDI
  // -- SURFACE MUST BE LOCKED TO BE READ FROM PROPERLY
  bm_dd_lPitch = ddsd_temp.lPitch ;

  // extract bitmap data
  source_ptr = bitmap->buffer ;

  // assign a pointer to the memory surface for manipulation
  dest_ptr = (UCHAR*)ddsd_temp.lpSurface ;

  // iterate thru each scanline and copy bitmap
  for( int index_y=0; index_y < (int)ddsd_temp.dwHeight; index_y++ )
  {
    // copy next line of data to destination
    memcpy( dest_ptr, source_ptr, ddsd_temp.dwWidth );

    // advance pointers
    dest_ptr   += ddsd_temp.lPitch ;//!! TAKE THE SURFACE DIMENSION INTO ACCOUNT !!
    source_ptr += bitmap->infoheader.biWidth ;

  } // end for index_y

  // unlock the surface 
  lpdds4->Unlock( 0 );

  // return success
  return 1 ;

}// DD_copyBitmap()

//=============================================================================

// create an RGB 8-bit palette
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
      {   myRed = 254 ;
        myGreen =  99 ;
         myBlue = 201 ; } // pink
    else if( index < 32 )
        myRed = 140 + (index-4)*4 ; // shades of red
    else if( index < 60 )
      {   myRed = 140 + (index-32)*4 ; 
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
      {  myRed = 60 + (index-200)*4 ; 
        myBlue = 140 + (index-200)*4 ; } // shades of violet
    else //( index < 256 )
        myRed = myBlue = 140 + (index-228)*4 ; // shades of magenta

    palette[index].peRed   = myRed   ;
    palette[index].peGreen = myGreen ;
	 palette[index].peBlue  = myBlue  ;

    // set flags
    palette[index].peFlags = PC_NOCOLLAPSE ;
  } // end for(index)

}// define_palette()

//=============================================================================

// OPEN A BITMAP FILE AND LOAD THE DATA INTO A BITMAP_CONTAINER
int Blackbox::Bitmap_Load_File( BITMAP_CTN_PTR bitmap, char *filename )
{
  int file_handle,  // the file handle
      index ;       // looping index

  UCHAR *temp_buffer = 0 ; // used to convert 24 bit images to 16 bit
  OFSTRUCT file_data ;     // the file data information

  // open the file if it exists
  if( (file_handle = OpenFile(filename, &file_data, OF_READ)) == -1 )
    return 0 ;

  // now load the bitmap file header
  _lread( file_handle, &(bitmap->fileheader), sizeof(BITMAPFILEHEADER) );

  // test if this is a bitmap file
  if( bitmap->fileheader.bfType != BITMAP_ID )
  {
    // close the file
    _lclose( file_handle );

    // return error
    return 0 ;
  }

  // NOW WE KNOW THIS IS A BITMAP, SO READ IN ALL THE SECTIONS

  // FIRST THE BITMAP INFOHEADER

  // now load the bitmap file header
  _lread( file_handle, &(bitmap->infoheader), sizeof(BITMAPINFOHEADER) );

  // now load the color palette if there is one
  if( bitmap->infoheader.biBitCount == 8 )
  {
    _lread(file_handle, &(bitmap->palette), MAX_COLORS*sizeof(PALETTEENTRY));

    // now set all the flags in the palette correctly and fix the reversed 
    // BGR RGBQUAD data format
    for( index=0; index < MAX_COLORS; index++ )
    {
      // reverse the red and green fields
      int temp_color                = bitmap->palette[index].peRed;
      bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
      bitmap->palette[index].peBlue = temp_color;
       
      // always set the flags word to this
      bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
    }
  }// if palette

  // finally the image data itself
  _lseek( file_handle, -(int)(bitmap->infoheader.biSizeImage), SEEK_END );

  // NOW READ IN THE IMAGE, IF THE IMAGE IS 8 OR 16 BIT THEN SIMPLY READ IT
  // BUT IF ITS 24 BIT THEN READ IT INTO A TEMPORARY AREA AND THEN CONVERT
  // IT TO A 16 BIT IMAGE

  if( bitmap->infoheader.biBitCount == 8 
      || bitmap->infoheader.biBitCount == 16
      || bitmap->infoheader.biBitCount == 24 )
  {
    // delete the last image if there was one
    if( bitmap->buffer )
      free( bitmap->buffer );

    // allocate the memory for the image
    if( !(bitmap->buffer = (UCHAR*)malloc(bitmap->infoheader.biSizeImage)) )
    {
      // close the file
      _lclose( file_handle );
      // return error
      return 0 ;
    }

    // now read it in
    _lread( file_handle, bitmap->buffer, bitmap->infoheader.biSizeImage );

  }
  else
      // serious problem
      return 0 ;

  // WRITE THE FILE INFO OUT 
  if( FILE *bmInfo = fopen( "bitMapInfo.txt", "a+" ) )
  {
    time_t ltime ;
    time( &ltime );
    fprintf( bmInfo, "\nCurrent time is %s", ctime(&ltime) );

    fprintf( bmInfo, "filename: %s \nfile size == %d \nimage size == %d \
\nwidth == %d \nheight == %d \nbitsperpixel == %d \nnum colors == %d \
\nnum important colors == %d \n",
             filename,
             bitmap->fileheader.bfSize,
             bitmap->infoheader.biSizeImage,
             bitmap->infoheader.biWidth,
             bitmap->infoheader.biHeight,
		       bitmap->infoheader.biBitCount,
             bitmap->infoheader.biClrUsed,
             bitmap->infoheader.biClrImportant );

    fclose( bmInfo );
  }

  // close the file
  _lclose( file_handle );

  // flip the bitmap
  Bitmap_Flip
    ( bitmap->buffer, 
      bitmap->infoheader.biWidth * (bitmap->infoheader.biBitCount/8), 
      bitmap->infoheader.biHeight
    );

  // return success
  return 1 ;

}// Bitmap_Load_File()

//=============================================================================

// RELEASE ALL MEMORY ASSOCIATED WITH THE BITMAP
int Blackbox::Bitmap_Unload_File( BITMAP_CTN_PTR bitmap )
{
  if( bitmap->buffer )
  {
    // release memory
    free( bitmap->buffer );
    // reset pointer
    bitmap->buffer = 0 ;

  } // end if

  // return success
  return 1 ;

}// Bitmap_Unload_File()

//=============================================================================

// FLIP BOTTOM-UP .BMP IMAGES
int Blackbox::Bitmap_Flip( UCHAR *image, int bytes_per_line, int height )
{
  UCHAR *buffer ; // used to perform the image processing
  int index ;     // looping index

  // allocate the temporary buffer
  if( !(buffer = (UCHAR *)malloc(bytes_per_line*height)) )
    return 0 ;

  // copy image to work area
  memcpy( buffer, image, bytes_per_line*height );

  // flip vertically
  for( index=0; index < height; index++ )
     memcpy( &image[((height-1) - index)*bytes_per_line],
             &buffer[index*bytes_per_line], bytes_per_line );

  // release the memory
  free( buffer );

  // return success
  return 1 ;

}// Bitmap_Flip()

//=============================================================================

// DRAW THE SENT TEXT ON THE SENT SURFACE USING THE COLOR IN THE PALETTE
int Blackbox::GDI_Draw_Text( char *text, int x, int y, int color,
									  LPDIRECTDRAWSURFACE4 lpdds )
{
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

}// GDI_Draw_Text()

//=============================================================================

DWORD Blackbox::Start_Clock()
{
  // START THE CLOCK, i.e. SAVE THE CURRENT COUNT
  // -- use in conjunction with Wait_Clock()

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
