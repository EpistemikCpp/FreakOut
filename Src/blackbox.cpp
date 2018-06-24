/**************************************************************************
 *
 *  Mark Sattolo (msattolo@sympatico.ca)
 * -----------------------------------------------
 *  $File: //depot/CppProj/VS6/FreakProj/Src/blackbox.cpp $
 *  $Revision: #6 $
 *  $Change: 82 $
 *  $DateTime: 2008/12/29 10:02:24 $
 *
 * ---------------------------------------------------------------------
 *	blackbox.cpp - DirectX and Utility functions - CLASS Blackbox
 *		Created 2001, based on code in 
 *		'Tricks of the Windows Game Programming Gurus', by Andre Lamothe
 * 
 **************************************************************************/
 
#define INITGUID  // NEED THIS FOR LINKER TO FIND SYMBOL 'IID_IDIRECTDRAW4'

#include "FreakProj.h"  // Windows & C++ includes
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

	// these define the general clipping rectangle
  min_clip_x = 0 ;                       
  max_clip_x = SCREEN_WIDTH-1 ;
  min_clip_y = 0 ;
  max_clip_y = SCREEN_HEIGHT-1 ;
}

//=============================================================================

// INITIALIZES DIRECTDRAW
bool Blackbox::DD_Init( HWND main_window, char * ball_file, 
                        int width, int height, int bpp     )
{
  LPDIRECTDRAW lpdd1 = NULL ; // used to query for the latest interface
  DDSCAPS2     ddscaps ;      // a direct draw surface capabilities struct

  // create object and test for error
  if( DD_OK != DirectDrawCreate(NULL, &lpdd1, NULL) )
  {
    MessageBox( main_window, "Could NOT create a DD object!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // query for version 4 of the interface
  if( DD_OK != lpdd1->QueryInterface(IID_IDirectDraw4, (LPVOID*)&lpdd) ) 
  {
    MessageBox( main_window, "Could NOT query the DD interface!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // set cooperation level to full screen exclusive mode
  if( DD_OK != lpdd->SetCooperativeLevel
                       ( main_window, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT ) )
  {
    MessageBox( main_window, "Could NOT set the DD Coop level!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // set the display mode
  if( DD_OK != lpdd->SetDisplayMode(width, height, bpp, 0, 0) )
  {
    MessageBox( main_window, "Could NOT set the DD display mode!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

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
  if( DD_OK != lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL) )
  {
    MessageBox( main_window, "Could NOT create the DD primary surface!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // query for the backbuffer, i.e the secondary surface
  ddscaps.dwCaps = DDSCAPS_BACKBUFFER ;
  if( DD_OK != lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback) )
  {
    MessageBox( main_window, "Could NOT attach the DD secondary surface!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // CREATE AND ATTACH PALETTE

  // define palette data
  definePalette8();

  // now create the palette object
  if( DD_OK != lpdd->CreatePalette( DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256,
    	                              palette, &lpddpal, NULL )  )
  {
    MessageBox( main_window, "Could NOT create the DD palette!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // attach the palette to the primary
  if( DD_OK != lpddsprimary->SetPalette(lpddpal) )
  {
    MessageBox( main_window, "Could NOT attach the DD palette!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // clear out both primary and secondary surfaces
  if( ! ( DD_Fill_Surface(lpddsprimary, 0) && DD_Fill_Surface(lpddsback, 0) ) )
  {
    MessageBox( main_window, "Could NOT fill the DD surfaces!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // attach a clipper to the back buffer
  RECT screen_rect = { 0, 0, width, height };
  if( NULL == ( lpddclipper = DD_Attach_Clipper(lpddsback, 1, &screen_rect) ) )
  {
    MessageBox( main_window, "Could NOT attach the DD clipper!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // LOAD THE BITMAP WITH THE BALL IMAGE & COPY THE IMAGE INTO THE DD SURFACE

  // load the 8-bit moving image
  if( ! Bitmap_Load_File(&ball_bm, ball_file) )
  {
    MessageBox( main_window, "Could NOT load the bitmap!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

  // create surface to hold image
  if( NULL == ( lpddsbkimg = DD_Create_Surface( ball_bm.infoheader.biWidth,
                                                ball_bm.infoheader.biHeight, 0 ) ) )
  {
    MessageBox( main_window, "Could NOT create the DD image surface!", "PROBLEM", MB_ICONWARNING );
    return FALSE ;
  }

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
void Blackbox::DD_Shutdown()
{
  // release the clipper first
  if( lpddclipper )
    lpddclipper->Release();

  // release the palette
  if( lpddpal )
    lpddpal->Release();

  // release the secondary surface
  if( lpddsback )
    lpddsback->Release();

  // release the primary surface
  if( lpddsprimary )
    lpddsprimary->Release();

  // finally, the main dd object
  if( lpdd )
    lpdd->Release();

}// DD_Shutdown()

//=============================================================================

// CREATE A CLIPPER FROM THE SENT CLIP LIST AND ATTACH IT TO THE SENT SURFACE
LPDIRECTDRAWCLIPPER Blackbox::DD_Attach_Clipper( LPDIRECTDRAWSURFACE4 lpdds,
                                                 int num_rects, LPRECT clip_list )
{
  LPDIRECTDRAWCLIPPER lpddclipper ; // pointer to the newly created dd clipper
  LPRGNDATA region_data ;           /* pointer to the region data that contains
                                       the header and clip list   */

  // first create the direct draw clipper
  if( (lpdd->CreateClipper(0, &lpddclipper, NULL)) != DD_OK )
    return NULL ;

  // NOW CREATE THE CLIP LIST FROM THE SENT DATA

  // first allocate memory for region data
  region_data = (LPRGNDATA)malloc( sizeof(RGNDATAHEADER) + num_rects*sizeof(RECT) );

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
  for( int index=0; index < num_rects; index++ )
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

  if( lpddclipper->SetClipList(region_data, 0) != DD_OK )
  {
    free( region_data ); // release memory and return error
    return NULL ;
  }

  // now attach the clipper to the surface
  if( lpdds->SetClipper(lpddclipper) != DD_OK )
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
void Blackbox::DD_Flip()
{
  // flip pages
  while( lpddsprimary->Flip(NULL, DDFLIP_WAIT) != DD_OK )
    ;// keep trying to flip while FAILED

}// DD_Flip()

//=============================================================================

bool Blackbox::DD_Fill_Surface( LPDIRECTDRAWSURFACE4 lpdds, int color )
{
  // clear out the structure and set the size field 
  DD_INIT_STRUCT( ddbltfx );

  // set the dwfillcolor field to the desired color
  ddbltfx.dwFillColor = color ; 

  // ready to blt to surface
  return( DD_OK == lpdds->Blt( NULL,        // ptr to dest rectangle
                               NULL,        // ptr to source surface, NA            
                               NULL,        // ptr to source rectangle, NA
                               DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC , // fill and wait                   
                               &ddbltfx )   // ptr to DDBLTFX structure
        );

}// DD_Fill_Surface()

//=============================================================================

// USE DIRECTDRAW TO DRAW A FILLED RECTANGLE
bool Blackbox::DD_Blt_Rect( int x1, int y1, int x2, int y2, int color,
                            LPDIRECTDRAWSURFACE4 lpdds )
{
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
  return( DD_OK == lpdds->Blt( &fill_area,  // ptr to dest rectangle
                               NULL,        // ptr to source surface, NA            
                               NULL,        // ptr to source rectangle, NA
                               DDBLT_COLORFILL | DDBLT_WAIT | DDBLT_ASYNC,  // fill and wait                   
                               &ddbltfx )   // ptr to DDBLTFX structure
        );

}// DD_Blt_Rect()

//=============================================================================

// CREATE AN OFFSCREEN PLAIN SURFACE
LPDIRECTDRAWSURFACE4 Blackbox::DD_Create_Surface( int width, int height,
                                                  int mem_flags, int color_key )
{
//	DDSURFACEDESC2 ddsd ;         // working description
	LPDIRECTDRAWSURFACE4 lpdds4 ;  // temporary surface
  
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
	if( FAILED( lpdd->CreateSurface(&ddsd, &lpdds4, 0) ) )
		return NULL ;
  
	// test if user wants a color key
	if( color_key >= 0 )
	{
    DDCOLORKEY ddcolkey ;
    ddcolkey.dwColorSpaceLowValue  = color_key ;
    ddcolkey.dwColorSpaceHighValue = color_key ;
    
    // now set the color key for source blitting
    lpdds4->SetColorKey( DDCKEY_SRCBLT, &ddcolkey );
	}
  
	// return the surface
	return lpdds4 ;

}// DD_Create_Surface()

//=============================================================================

bool Blackbox::DD_Draw_Surface( int x, int y,     // position to draw at
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
  dest_rect.right  = x +  width - 1  ;
  dest_rect.bottom = y + height - 1 ;

  // fill in the source rect
  source_rect.left    = 0 ;
  source_rect.top     = 0 ;
  source_rect.right   =  width - 1  ;
  source_rect.bottom  = height - 1 ;

  // test transparency flag
  if( transparent )
  {
    // enable color key blit
    // blt to destination surface
    if( FAILED(lpddsback->Blt( &dest_rect, lpddsbkimg, &source_rect,
                               (DDBLT_WAIT | DDBLT_KEYSRC), 0 )) )
      return FALSE ;
  }
  else
    {
      // perform blit without color key
      // blt to destination surface
      if( FAILED(lpddsback->Blt( &dest_rect, lpddsbkimg, &source_rect,
                                 (DDBLT_WAIT), 0 )) )
        return FALSE ;
    }
  
  // return success
  return TRUE ;

}// DD_Draw_Surface()

//=============================================================================

// COPY A BITMAP FROM A BITMAP_CONTAINER TO A DD SURFACE
void Blackbox::DD_copyBitmap( pBITMAP_CONTAINER bitmap,    // struct with the image data
                              LPDIRECTDRAWSURFACE4 lpdds4, // surface to hold data
                              int width, int height )      // dimensions of image
{
  UCHAR *source_ptr, *dest_ptr ; // working pointers

  DDSURFACEDESC2 ddsd_temp ;  //  direct draw surface description 

  // set size of the structure
  ddsd_temp.dwSize = sizeof ddsd_temp ;

  // set dimensions of the new bitmap surface
  ddsd_temp.dwWidth  = width  ; // 72
  ddsd_temp.dwHeight = height ; // 80

  // lock the display surface
  lpdds4->Lock( 0, &ddsd_temp, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0 );

  // extract bitmap data
  source_ptr = bitmap->buffer ;

  // assign a pointer to the memory surface for manipulation
  dest_ptr = (UCHAR*)ddsd_temp.lpSurface ;

  // iterate thru each scanline and copy bitmap
  for( int index_y=0; index_y < (int)ddsd_temp.dwHeight; index_y++ )
  {
    // copy next line of data to destination
    memcpy( dest_ptr, source_ptr, ddsd_temp.dwWidth );

    // advance the pointers
    dest_ptr   += ddsd_temp.lPitch ;//!! TAKE THE SURFACE DIMENSION INTO ACCOUNT !!
    source_ptr += bitmap->infoheader.biWidth ;

  }

  // unlock the surface 
  lpdds4->Unlock( 0 );

}// DD_copyBitmap()

//=============================================================================

// create an RGB 8-bit palette
void Blackbox::definePalette8()
{
  int myRed, myGreen, myBlue ;

  // set all values to zero first - defensive programming
  memset( palette, 0, 256 * sizeof(PALETTEENTRY) );
  
  // SET COLORS
  for( int index = 0; index < 256; index++ )
  {
    // a little brightness for each range
    myRed = myGreen = myBlue = 8 ; 

    if( index == 0 ) 
      myRed = myGreen = myBlue = 1 ; // black
    else if( index == 1 )
        myRed = myGreen = myBlue = 87 ; // grey
    else if( index == 2 )
        myRed = myGreen = myBlue = 254 ; // white
    else if( index == 3 )
      { myRed   = 254 ;
        myGreen =  99 ;
        myBlue  = 201 ; } // pink
    else if( index < 32 )
        myRed = 140 + ( (index-4) << 2 ); // shades of red
    else if( index < 60 )
      { myRed   = 140 + ( (index-32) << 2 ); 
        myGreen =  60 + ( (index-32) << 2 ); } // shades of orange
    else if( index < 88 )
        myRed = myGreen = 140 + ( (index-60) << 2 ); // shades of yellow
    else if( index < 116 )
        myGreen = 140 + ( (index-88) << 2 ); // shades of bright green
    else if( index < 144 )
        myGreen =  60 + ( (index-116) << 2 ); // shades of dark green
    else if( index < 172 )
        myGreen = myBlue = 140 + ( (index-144) << 2 ); // shades of cyan
    else if( index < 200 )
        myBlue = 140 + ( (index-172) << 2 ); // shades of blue
    else if( index < 228 )
      { myRed  =  60 + ( (index-200) << 2 ); 
        myBlue = 140 + ( (index-200) << 2 ); } // shades of violet
    else //( index < 256 )
        myRed = myBlue = 140 + ( (index-228) << 2 ); // shades of magenta

    palette[index].peRed   = (UCHAR)myRed   ;
    palette[index].peGreen = (UCHAR)myGreen ;
	  palette[index].peBlue  = (UCHAR)myBlue  ;

    // set flags
    palette[index].peFlags = PC_NOCOLLAPSE ;

  }// for( index )

}// define_palette8()

//=============================================================================

// OPEN A BITMAP FILE AND LOAD THE DATA INTO A BITMAP_CONTAINER
bool Blackbox::Bitmap_Load_File( pBITMAP_CONTAINER bitmap, char *filename )
{
  bool success = TRUE ;
  int file_handle ;

  OFSTRUCT file_data ;     // the file data information

  // open the file if it exists
  if( ( file_handle = OpenFile(filename, &file_data, OF_READ) ) == -1 )
    success = FALSE ;

  if( success )
  {
    // now load the bitmap file header
    _lread( file_handle, &(bitmap->fileheader), sizeof(BITMAPFILEHEADER) );
    
    // test if this is a bitmap file
    if( bitmap->fileheader.bfType != BITMAP_ID )
    {
      // close the file
      _lclose( file_handle );
      
      // error
      success = FALSE ;
    }
  }
  
  // NOW WE KNOW THIS IS A BITMAP, SO READ IN ALL THE SECTIONS
  if( success )
  {
    // load the file header
    _lread( file_handle, &(bitmap->infoheader), sizeof(BITMAPINFOHEADER) );

    // now load the color palette if there is one
    if( bitmap->infoheader.biBitCount == 8 )
    {
      _lread( file_handle, &(bitmap->palette), MAX_COLORS*sizeof(PALETTEENTRY) );

      // now set all the flags in the palette correctly 
      // and fix the reversed BGR RGBQUAD data format
      for( int index=0; index < MAX_COLORS; index++ )
      {
        // reverse the red and green fields
        UCHAR temp_color                = bitmap->palette[index].peRed;
        bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
        bitmap->palette[index].peBlue = temp_color;
       
        // always set the flags word to this
        bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
      }
    }// got the palette
    
    // finally the image data itself
	  // - move backwards from the end of the file to the start of the data bits
    _lseek( file_handle, -(int)(bitmap->infoheader.biSizeImage), SEEK_END );
    
    // read in the image
    if( bitmap->infoheader.biBitCount == 8 
        || bitmap->infoheader.biBitCount == 16 || bitmap->infoheader.biBitCount == 24 )
    {
      // delete the last image if there was one
      if( bitmap->buffer )
        free( bitmap->buffer );
      
      // allocate the memory for the image
      if( !( bitmap->buffer = (UCHAR*)malloc(bitmap->infoheader.biSizeImage) ) )
      {
        // close the file
        _lclose( file_handle );
        
        // error
        success = FALSE ;
      }
      else
          // read it in
          _lread( file_handle, bitmap->buffer, bitmap->infoheader.biSizeImage );
    
    }// got proper bit count
    else
        // serious problem
        success = FALSE ;
  }

  if( success )
  {
    // DEBUG: DISPLAY FILE INFO
    if( FILE *bmInfo = fopen( "Info\\bitMapInfo.txt", "a+" ) )
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
  }

  if( success )
  {
    // close the file
    _lclose( file_handle );

    // flip the bitmap
    Bitmap_Flip( bitmap->buffer, 
                 bitmap->infoheader.biWidth * (bitmap->infoheader.biBitCount/8), 
                 bitmap->infoheader.biHeight  );

    // return success
    return TRUE ;
  }

  fprintf( stderr, "Could NOT load the bitmap!" );
  return FALSE ;
  
}// Bitmap_Load_File()

//=============================================================================

// RELEASE ALL MEMORY ASSOCIATED WITH THE BITMAP
void Blackbox::Bitmap_Unload_File( pBITMAP_CONTAINER bitmap )
{
  if( bitmap->buffer )
  {
    // release memory
    free( bitmap->buffer );
    // reset pointer
    bitmap->buffer = 0 ;
  }

}// Bitmap_Unload_File()

//=============================================================================

// FLIP BOTTOM-UP .BMP IMAGES
bool Blackbox::Bitmap_Flip( UCHAR *image, int bytes_per_line, int height )
{
  UCHAR *buffer ; // used to perform the image processing

  // allocate the temporary buffer
  if( !(buffer = (UCHAR *)malloc(bytes_per_line*height)) )
    return FALSE ;

  // copy image to work area
  memcpy( buffer, image, bytes_per_line*height );

  // flip vertically
  for( int index=0; index < height; index++ )
     memcpy( &image[((height-1) - index)*bytes_per_line],
             &buffer[index*bytes_per_line], bytes_per_line );

  // release the memory
  free( buffer );

  // return success
  return TRUE ;

}// Bitmap_Flip()

//=============================================================================

// DRAW THE SENT TEXT ON THE SENT SURFACE USING THE COLOR IN THE PALETTE
bool Blackbox::GDI_Draw_Text( char *text, int x, int y, int color,
		  												LPDIRECTDRAWSURFACE4 lpdds )
{
  HDC wdc ; // the working dc

  // get the dc from surface
  if( lpdds->GetDC(&wdc) != DD_OK )
    return FALSE ;

  // set the colors for the text
  SetTextColor( wdc, RGB( palette[color].peRed, palette[color].peGreen, palette[color].peBlue ) );

  // set background mode to transparent so black isn't copied
  SetBkMode( wdc, TRANSPARENT );

  // draw the text
  TextOut( wdc, x, y, text, strlen(text) );

  // release the dc
  lpdds->ReleaseDC( wdc );

  // return success
  return TRUE ;

}// GDI_Draw_Text()
