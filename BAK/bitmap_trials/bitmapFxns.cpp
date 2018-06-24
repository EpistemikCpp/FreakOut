// bitmapFxns.cpp
// -- from DEMO7_13.CPP


int Load_Bitmap_File( BITMAP_CTN_PTR bitmap, char *filename )
{
// THIS FUNCTION OPENS A BITMAP FILE AND LOADS THE DATA INTO BITMAP

int file_handle,  // the file handle
    index ;       // looping index

UCHAR *temp_buffer = 0 ; // used to convert 24 bit images to 16 bit
OFSTRUCT file_data ;     // the file data information

// open the file if it exists
if( (file_handle = OpenFile(filename, &file_data, OF_READ)) == -1 )
  return 0 ;

// now load the bitmap file header
_lread( file_handle, &bitmap->bitmapfileheader, sizeof(BITMAPFILEHEADER) );

// test if this is a bitmap file
if( bitmap->bitmapfileheader.bfType != BITMAP_ID )
{
   // close the file
   _lclose( file_handle );

   // return error
   return 0 ;
}

// NOW WE KNOW THIS IS A BITMAP, SO READ IN ALL THE SECTIONS

// FIRST THE BITMAP INFOHEADER

// now load the bitmap file header
_lread( file_handle, &bitmap->bitmapinfoheader, sizeof(BITMAPINFOHEADER) );

// now load the color palette if there is one
if( bitmap->bitmapinfoheader.biBitCount == 8 )
{
   _lread(file_handle, &bitmap->palette, MAX_COLORS_PALETTE*sizeof(PALETTEENTRY));

   // now set all the flags in the palette correctly and fix the reversed 
   // BGR RGBQUAD data format
   for( index=0; index < MAX_COLORS_PALETTE; index++ )
   {
     // reverse the red and green fields
     int temp_color                = bitmap->palette[index].peRed;
     bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
     bitmap->palette[index].peBlue = temp_color;
       
     // always set the flags word to this
     bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
   }
}

// finally the image data itself
_lseek( file_handle, -(int)(bitmap->bitmapinfoheader.biSizeImage), SEEK_END );

// NOW READ IN THE IMAGE, IF THE IMAGE IS 8 OR 16 BIT THEN SIMPLY READ IT
// BUT IF ITS 24 BIT THEN READ IT INTO A TEMPORARY AREA AND THEN CONVERT
// IT TO A 16 BIT IMAGE

if( bitmap->bitmapinfoheader.biBitCount == 8 
    || bitmap->bitmapinfoheader.biBitCount == 16
    || bitmap->bitmapinfoheader.biBitCount == 24 )
{
   // delete the last image if there was one
   if( bitmap->buffer )
     free( bitmap->buffer );

   // allocate the memory for the image
   if( !(bitmap->buffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)) )
   {
      // close the file
      _lclose( file_handle );
      // return error
      return 0 ;
   }

   // now read it in
   _lread( file_handle, bitmap->buffer, bitmap->bitmapinfoheader.biSizeImage );

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
           bitmap->bitmapfileheader.bfSize,
           bitmap->bitmapinfoheader.biSizeImage,
           bitmap->bitmapinfoheader.biWidth,
           bitmap->bitmapinfoheader.biHeight,
		     bitmap->bitmapinfoheader.biBitCount,
           bitmap->bitmapinfoheader.biClrUsed,
           bitmap->bitmapinfoheader.biClrImportant );

  fclose( bmInfo );
}

// close the file
_lclose( file_handle );

// flip the bitmap
Flip_Bitmap
   ( bitmap->buffer, 
     bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8), 
     bitmap->bitmapinfoheader.biHeight
   );

// return success
return 1 ;

}// Load_Bitmap_File()

///////////////////////////////////////////////////////////

int Unload_Bitmap_File( BITMAP_CTN_PTR bitmap )
{
  // THIS FUNCTION RELEASES ALL MEMORY ASSOCIATED WITH "BITMAP"
  if( bitmap->buffer )
  {
    // release memory
    free( bitmap->buffer );
    // reset pointer
    bitmap->buffer = 0 ;

  } // end if

  // return success
  return 1 ;

}// Unload_Bitmap_File()

///////////////////////////////////////////////////////////

int Flip_Bitmap( UCHAR *image, int bytes_per_line, int height )
{
 // THIS FUNCTION IS USED TO FLIP BOTTOM-UP .BMP IMAGES

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

}// Flip_Bitmap()

///////////////////////////////////////////////////////////////

LPDIRECTDRAWCLIPPER DD_Attach_Clipper( LPDIRECTDRAWSURFACE4 lpdds,
                                       int num_rects, LPRECT clip_list )
{
// this function creates a clipper from the sent clip list 
// and attaches it to the sent surface

int index ;                    // looping var
LPDIRECTDRAWCLIPPER lpddclip ; // pointer to the newly created dd clipper
LPRGNDATA region_data ;        // pointer to the region data that contains
                               // the header and clip list

// first create the direct draw clipper
if( FAILED(lpdd4->CreateClipper(0, &lpddclip, 0)) )
  return(0);

// now create the clip list from the sent data

// first allocate memory for region data
region_data = (LPRGNDATA)malloc( sizeof(RGNDATAHEADER)+num_rects*sizeof(RECT) );

// now copy the rects into region data
memcpy( region_data->Buffer, clip_list, sizeof(RECT)*num_rects );

// set up fields of header
region_data->rdh.dwSize     = sizeof( RGNDATAHEADER );
region_data->rdh.iType      = RDH_RECTANGLES ;
region_data->rdh.nCount     = num_rects ;
region_data->rdh.nRgnSize   = num_rects*sizeof(RECT) ;

region_data->rdh.rcBound.left    =  64000 ;
region_data->rdh.rcBound.top     =  64000 ;
region_data->rdh.rcBound.right   = -64000 ;
region_data->rdh.rcBound.bottom  = -64000 ;

// find bounds of all clipping regions
for( index=0; index<num_rects; index++ )
{
    // test if the next rectangle unioned with the current bound is larger
    if (clip_list[index].left < region_data->rdh.rcBound.left)
       region_data->rdh.rcBound.left = clip_list[index].left ;

    if (clip_list[index].right > region_data->rdh.rcBound.right)
       region_data->rdh.rcBound.right = clip_list[index].right ;

    if (clip_list[index].top < region_data->rdh.rcBound.top)
       region_data->rdh.rcBound.top = clip_list[index].top ;

    if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
       region_data->rdh.rcBound.bottom = clip_list[index].bottom ;

}

// now we have computed the bounding rectangle region and set up the data
// now let's set the clipping list

if( FAILED(lpddclip->SetClipList(region_data, 0)) )
{
   // release memory and return error
   free( region_data );
   return 0 ;
}

// now attach the clipper to the surface
if( FAILED(lpdds->SetClipper(lpddclip)) )
{
   // release memory and return error
   free( region_data );
   return 0 ;
}

// all is well, so release memory and send back the pointer to the new clipper
free( region_data );
return lpddclip ;

}// DD_Attach_Clipper()

///////////////////////////////////////////////////////////   

int DD_Fill_Surface( LPDIRECTDRAWSURFACE4 lpdds, int color )
{
  DDBLTFX ddbltfx ; // this contains the DDBLTFX structure

  // clear out the structure and set the size field 
  DDRAW_INIT_STRUCT( ddbltfx );

  // set the dwfillcolor field to the desired color
  ddbltfx.dwFillColor = color ; 

  // ready to blt to surface
  lpdds->Blt( 0,        // ptr to dest rectangle
              0,        // ptr to source surface, NA            
              0,        // ptr to source rectangle, NA
              DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
              &ddbltfx );  // ptr to DDBLTFX structure

  // return success
  return 1 ;
}
// DD_Fill_Surface()

///////////////////////////////////////////////////////////////

int DD_Draw_Surface( LPDIRECTDRAWSURFACE4 source, // source surface to draw
                     int x, int y,                // position to draw at
                     int width, int height,       // size of source surface
                     LPDIRECTDRAWSURFACE4 dest,   // dest surface to draw to
                     int transparent = 1  )       // transparency flag
{
RECT dest_rect,    // the destination rectangle
     source_rect ; // the source rectangle                             

// fill in the destination rect
dest_rect.left   = x ;
dest_rect.top    = y ;
dest_rect.right  = x +  width - 1 ;
dest_rect.bottom = y + height - 1 ;

// fill in the source rect
source_rect.left    = 0 ;
source_rect.top     = 0 ;
source_rect.right   =  width - 1 ;
source_rect.bottom  = height - 1 ;

// test transparency flag
if( transparent )
{
  // enable color key blit
  // blt to destination surface
  if( FAILED(dest->Blt( &dest_rect, source, &source_rect,
                        (DDBLT_WAIT | DDBLT_KEYSRC), 0 )) )
    return 0 ;
}
else
  {
    // perform blit without color key
    // blt to destination surface
    if( FAILED(dest->Blt( &dest_rect, source, &source_rect,
                          (DDBLT_WAIT), 0 )) )
      return 0 ;
  }

// return success
return 1 ;

}// DD_Draw_Surface()

///////////////////////////////////////////////////////////////

int copyBitmapToSurface( BITMAP_CTN_PTR bitmap,   // struct with the image data
                         LPDIRECTDRAWSURFACE4 lpdds4, // surface to hold data
                         int cx, int cy  )            // cell to scan image from
{
// COPY A BITMAP FROM A BITMAP_CONTAINER TO A DD SURFACE

UCHAR *source_ptr, *dest_ptr ; // working pointers

DDSURFACEDESC2 ddsd_temp ;  //  direct draw surface description 

// GET THE ADDR TO DESTINATION SURFACE MEMORY

// set size of the structure
ddsd_temp.dwSize = sizeof ddsd_temp ;

  // set dimensions of the new bitmap surface
  ddsd_temp.dwWidth  = fg_bm_width  ; // 72
  ddsd_temp.dwHeight = fg_bm_height ; // 80

// lock the display surface
lpdds4->Lock( 0, &ddsd_temp, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0 );

  // display this value in GameMain() using GDI
  // -- SURFACE MUST BE LOCKED TO BE READ FROM PROPERLY
  fg_lPitch = ddsd_temp.lPitch ;

// compute position to start scanning bits from
// EACH CELL OF THE TEMPLATED BITMAP HAS A 1-BIT BORDER SURROUNDING IT
cx = cx*(  ddsd_temp.dwWidth + 1 ) + 1 ;
cy = cy*( ddsd_temp.dwHeight + 1 ) + 1 ;

// extract bitmap data
source_ptr = bitmap->buffer + (cy*bitmap->bitmapinfoheader.biWidth) + cx ;

// assign a pointer to the memory surface for manipulation
dest_ptr = (UCHAR*)ddsd_temp.lpSurface ;

// iterate thru each scanline and copy bitmap
for( int index_y=0; index_y < (int)ddsd_temp.dwHeight; index_y++ )
{
    // copy next line of data to destination
    memcpy( dest_ptr, source_ptr, ddsd_temp.dwWidth );

    // advance pointers
    dest_ptr   += ddsd_temp.lPitch ;//!! TAKE THE SURFACE DIMENSION INTO ACCOUNT !!
    source_ptr += bitmap->bitmapinfoheader.biWidth ;

} // end for index_y

// unlock the surface 
lpdds4->Unlock( 0 );

// return success
return 1 ;

}// copyBitmapToSurface()

///////////////////////////////////////////////////////////////

LPDIRECTDRAWSURFACE4 DD_Create_Surface( int width, int height, 
                                        int mem_flags, int color_key = 0 )
{
 // THIS FUNCTION CREATES AN OFFSCREEN PLAIN SURFACE

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
 ddsd.ddsCaps.dwCaps = /*DDSCAPS_OFFSCREENPLAIN*/DDSCAPS_VIDEOMEMORY 
                       | mem_flags ;

 // create the surface
 if( FAILED(lpdd4->CreateSurface(&ddsd, &lpdds, 0)) )
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

///////////////////////////////////////////////////////////

int Draw_Text_GDI( char *text, int x, int y, COLORREF color,
                   LPDIRECTDRAWSURFACE4 lpdds4 )
{
// DRAWS THE SENT TEXT ON THE SENT SURFACE 
// USING COLOR INDEX AS THE COLOR IN THE PALETTE

HDC xdc ; // the working dc

// get the dc from surface
if( FAILED(lpdds4->GetDC(&xdc)) )
  return 0 ;

// set the colors for the text
SetTextColor( xdc, color );

// set background mode to transparent so black isn't copied
SetBkMode( xdc, TRANSPARENT );

// draw the text
TextOut( xdc, x, y, text, strlen(text) );

// release the dc
lpdds4->ReleaseDC( xdc );

// return success
return 1 ;

} // end Draw_Text_GDI

///////////////////////////////////////////////////////////////

int Game_Main()
{
// THIS IS THE MAIN LOOP OF THE GAME, DO ALL YOUR PROCESSING HERE

// lookup for proper walking sequence
static int animation_seq[4] = {0, 1, 0, 2};

int index ; // general looping variable

// make sure this isn't executed again
if( window_closed )
  return 0 ;

// for now test if user is hitting ESC and send WM_CLOSE
if( KEYDOWN(VK_ESCAPE) )
{
   PostMessage( main_window_handle, WM_CLOSE, 0, 0 );
   window_closed = true ;
} // end if

// copy background to back buffer
DD_Draw_Surface( lpddsbkimg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, lpddsback, 0 );    

// move objects around
for( index=0; index < 3; index++ )
{
    // move each object to the right at its given velocity
    aliens[index].x += aliens[index].velocity ;

    // test if off screen edge, and wrap around
    if( aliens[index].x > SCREEN_WIDTH )
      aliens[index].x = -(fg_bm_width) ;

    // animate bot
    if( ++aliens[index].counter >= (8 - aliens[index].velocity) )
    {
       // reset counter
       aliens[index].counter = 0 ;
       // advance to next frame
       if( ++aliens[index].current_frame > 3 )
         aliens[index].current_frame = 0 ;
    }
}

// draw all the bots
for( index=0; index < 3; index++ )
{
    // draw objects
    DD_Draw_Surface
      ( aliens[index].frames[ animation_seq[aliens[index].current_frame] ], 
        aliens[index].x, aliens[index].y, fg_bm_width, fg_bm_height, lpddsback );
} // end for index

char infotext[64] = {0};
sprintf( infotext, "DD bg surface lPitch == %d , fg surface lPitch == %d",
         bg_lPitch, fg_lPitch  );
Draw_Text_GDI( infotext, (int)30, (int)(SCREEN_HEIGHT-30),
               0x0000ffff/*yellow*/, lpddsback );

// flip pages
while( FAILED(lpddsprimary->Flip(0, DDFLIP_WAIT)) )
  ;// keep trying to flip

// wait a while
Sleep( PAUSE_C ) ;

// return success or failure or your own return code here
return 1 ;

}// Game_Main()

////////////////////////////////////////////////////////////////////////////

int Game_Init()
{
  // this is called once after the initial window is created and
  // before the main event loop is entered, do all your initialization here
  LPDIRECTDRAW lpdd_temp ;

  // first create base IDirectDraw interface
  errno = DirectDrawCreate( 0, &lpdd_temp, 0 );
  if( errno != DD_OK ) return 0 ;

  // now query for IDirectDraw4
  errno = lpdd_temp->QueryInterface( IID_IDirectDraw4, (LPVOID*)&lpdd4 );
  if( errno != DD_OK ) return 0 ;

  // set cooperation to full screen
  errno = lpdd4->SetCooperativeLevel
                  ( main_window_handle, 
                    DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT 
                   // DDSCL_NORMAL 
                   );
  if( errno != DD_OK ) return 0 ;

  // set display mode to 640x480x8
  errno = lpdd4->SetDisplayMode( 640, 480, SCREEN_BPP, 0, 0 );
  if( errno != DD_OK ) return 0 ;

// WE NEED A COMPLEX SURFACE SYSTEM WITH A PRIMARY AND BACKBUFFER

// clear ddsd and set size
DDRAW_INIT_STRUCT( ddsd ); 

// enable valid fields
ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT ;

// SET THE BACKBUFFER COUNT FIELD TO 1, use 2 for triple buffering
ddsd.dwBackBufferCount = 1 ;

// request a complex, flippable
ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP ;

  // CREATE THE PRIMARY SURFACE
  errno = lpdd4->CreateSurface( &ddsd, &lpddsprimary, 0 );
  if( errno != DD_OK ) return 0 ;

// NOW QUERY FOR ATTACHED BACK BUFFER SURFACE FROM THE PRIMARY SURFACE

// this line is needed by the call
ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER ;

  // get the attached back buffer surface
  errno = lpddsprimary->GetAttachedSurface( &ddsd.ddsCaps, &lpddsback );
  if( errno != DD_OK ) return 0 ;

  // create the palette object
  errno = lpdd4->CreatePalette
                  ( DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE, 
                    palette, &lpddpal, 0 );
  if( errno != DD_OK ) return 0 ;

  // finally attach the palette to the primary surface
  errno = lpddsprimary->SetPalette( lpddpal );
  if( errno != DD_OK ) return 0 ;

// set clipper up on back buffer since that's where well clip
RECT screen_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
lpddclipper = DD_Attach_Clipper( lpddsback, 1, &screen_rect );

// load the 8-bit background image into the BITMAP_CONTAINER structure
if( !Load_Bitmap_File(&bitmap, "alley8.bmp") ) return 0 ;

  // load it's palette into directdraw
  errno = lpddpal->SetEntries( 0, 0, MAX_COLORS_PALETTE, bitmap.palette );
  if( errno != DD_OK ) return 0 ;

// clean the surfaces
DD_Fill_Surface( lpddsprimary, 0 );
DD_Fill_Surface( lpddsback, 0 );

// create the buffer to hold the background
lpddsbkimg = DD_Create_Surface( SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1 );

// COPY THE BACKGROUND BITMAP IMAGE TO THE BACKGROUND SURFACE 

// lock the surface
lpddsbkimg->Lock( 0, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 0 );

  bg_lPitch = ddsd.lPitch ;

// get video pointer to background surfce
UCHAR *image_buffer = (UCHAR*)ddsd.lpSurface ;

// test if memory is linear
if( ddsd.lPitch == SCREEN_WIDTH )
{
  // copy memory from the bitmap struct to the background surface
  memcpy((void*)image_buffer, (void*)bitmap.buffer, SCREEN_WIDTH*SCREEN_HEIGHT);
}
else // non-linear
  { 
    // make copy of source and destination addresses
    UCHAR *dest_ptr = image_buffer  ;
    UCHAR *src_ptr  = bitmap.buffer ;

    // memory is non-linear, copy line by line
    for( int y=0; y < SCREEN_HEIGHT; y++ )
    {
       // copy line
       memcpy( (void *)dest_ptr, (void *)src_ptr, SCREEN_WIDTH ) ;
       // advance pointers to next line
       dest_ptr += ddsd.lPitch ;
       src_ptr += SCREEN_WIDTH ;
    }
  } // end else

// now unlock the background surface
  errno = lpddsbkimg->Unlock( 0 );
  if( errno != DD_OK ) return 0 ;

// unload the bitmap file, we no longer need it
Unload_Bitmap_File( &bitmap );

// seed random number generator
srand( GetTickCount() );

// INITIALIZE ALL THE ALIENS

// alien on level 1 of complex
aliens[0].x              = SCREEN_WIDTH/5 ;
aliens[0].y              = 116 - fg_bm_height ;                  
aliens[0].velocity       = 2 + rand()%rand_mod ;
aliens[0].current_frame  = 0 ;             
aliens[0].counter        = 0 ;       

// alien on level 2 of complex
aliens[1].x              = SCREEN_WIDTH/4 ;
aliens[1].y              = 246 - fg_bm_height ;                  
aliens[1].velocity       = 2 + rand()%rand_mod ;
aliens[1].current_frame  = 0 ;             
aliens[1].counter        = 0 ;  

// alien on level 3 of complex
aliens[2].x              = SCREEN_WIDTH/3 ;
aliens[2].y              = 382 - fg_bm_height ;                  
aliens[2].velocity       = 2 + rand()%rand_mod ;
aliens[2].current_frame  = 0 ;             
aliens[2].counter        = 0 ;  

// NOW LOAD THE BITMAP CONTAINING THE ALIEN IMAGERY
// THEN SCAN THE IMAGES OUT INTO THE SURFACES OF ALIEN[0]
// AND COPY THEN INTO THE OTHER TWO, BE CAREFUL OF REFERENCE COUNTS!

// load the 8-bit moving image
if( !Load_Bitmap_File(&bitmap, "dedsp0.bmp") )
  return 0 ;

// create each surface and load bits
for( int index = 0; index < 3; index++ )
{
    // create surface to hold image
    aliens[0].frames[index] = DD_Create_Surface( fg_bm_width, fg_bm_height, 0 );

    // now load bits...
    copyBitmapToSurface( &bitmap,                 // bitmap file to scan image data from
                         aliens[0].frames[index], // surface to hold data
                         index, 0  );             // cell to scan image from    

} // end for index

// unload the bitmap file, we no longer need it
Unload_Bitmap_File( &bitmap );

/* now for the tricky part. There is no need to create more surfaces with
   the same data, so I'm going to copy the surface pointers member for member
   to each alien however, be careful, since the reference counts do NOT go up,
   you still only need to release() each surface once!  */
for( index = 0; index < 3; index++ )
  aliens[2].frames[index] = aliens[1].frames[index] = aliens[0].frames[index];

// return success or failure or your own return code here
return 1 ;

} // Game_Init()

//////////////////////////////////////////////////////////////////

int Game_Shutdown()
{
// KILL ALL THE SURFACES

// first the palette
if (lpddpal)
{
   lpddpal->Release();
   lpddpal = 0 ;
}

// now the primary surface
if (lpddsprimary)
{
   lpddsprimary->Release();
   lpddsprimary = 0 ;
}

// now blow away the IDirectDraw4 interface
if (lpdd4)
{
   lpdd4->Release();
   lpdd4 = 0 ;
}

// return success or failure or your own return code here
return 1 ;

}// Game_Shutdown()

////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
 // THIS IS THE MAIN MESSAGE HANDLER OF THE SYSTEM
 PAINTSTRUCT ps;		// used in WM_PAINT

 // what is the message?
 switch( msg )
 {	
	case WM_CREATE: 
   {
	  // do initialization stuff here
     // return success
	  return 0 ;
   }
    break;
   
	case WM_PAINT: 
   {
	  // simply validate the window 
     BeginPaint( hwnd,&ps );	 
     // end painting
     EndPaint( hwnd,&ps );
     // return success
	  return 0 ;
   }
    break;

	case WM_DESTROY: 
   {
		// kill the application, this sends a WM_QUIT message 
		PostQuitMessage( 0 );
      // return success
		return 0 ;
   }
    break;

	default: break;

 } // end switch

 // process any messages that we didn't take care of 
 return DefWindowProc( hwnd, msg, wparam, lparam ) ;

}// WindowProc()

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hprevinstance,
					     LPSTR lpcmdline, int ncmdshow )
{
 WNDCLASSEX  winclass ; // this will hold the class we create
       HWND	 hwnd ;	   // generic window handle
        MSG	 msg ;		// generic message

// first fill in the window class stucture
winclass.cbSize         = sizeof( WNDCLASSEX );
winclass.style			   = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW ;
winclass.lpfnWndProc	   = WindowProc ;
winclass.cbClsExtra		= 0 ;
winclass.cbWndExtra		= 0 ;
winclass.hInstance		= hinstance;
winclass.hIcon			   = LoadIcon( 0, IDI_APPLICATION );
winclass.hCursor		   = LoadCursor( 0, IDC_ARROW ); 
winclass.hbrBackground	= (HBRUSH)GetStockObject( BLACK_BRUSH );
winclass.lpszMenuName	= 0 ;
winclass.lpszClassName	= WINDOW_CLASS_NAME ;
winclass.hIconSm        = LoadIcon( 0, IDI_APPLICATION );

// register the window class
if( !RegisterClassEx(&winclass) )
  return 0 ;

// create the window
if( !(hwnd = CreateWindowEx(
                      0,                     // extended style
                      WINDOW_CLASS_NAME,     // class
						    "DirectDraw 8-Bit Blitting Demo",  // title
						    WS_POPUP//*/ WS_OVERLAPPEDWINDOW
                      | WS_VISIBLE ,
					 	    0, 0,/*40,30,*/  // initial x,y
						    SCREEN_WIDTH, SCREEN_HEIGHT,  // initial width, height
						    0,	        // handle to parent 
						    0,	        // handle to menu
						    hinstance,   // instance of this application
						    0 ))	        // extra creation parms
  )return 0 ;

// save main window handle
main_window_handle = hwnd ;

ShowCursor( false );

// initialize game here
Game_Init();

// enter main event loop
while( true )
{
    // test if there is a message in queue, if so get it
	if( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) )
	{ 
	  // test if this is a quit
     if( msg.message == WM_QUIT )
       break ;
	
	  // translate any accelerator keys
	  TranslateMessage( &msg );

	  // send the message to the window proc
	  DispatchMessage( &msg );
	} // end if
    
   // main game processing goes here
   Game_Main();
       
} // end while

// closedown game here
Game_Shutdown();

ShowCursor( true );

// return to Windows like this
return( msg.wParam );

} // end WinMain