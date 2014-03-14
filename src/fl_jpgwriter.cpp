#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "fl_jpgwriter.h"
#include "jpeglib.h"

bool fl_image_write_to_jpeg( Fl_Image* refimg, const char* refpath, int q  )
{
    if ( ( refimg == NULL ) || ( refpath == NULL ) || ( q <= 1 ) )
        return false;

    if ( ( refimg->w() == 0 ) || ( refimg->h() == 0 ) )
        return false;

    const uchar* refpix = (const uchar*)&refimg->data()[0];

    return array_write_to_jpeg( refpix, refimg->w(), refimg->h(), refimg->d(), refpath, q );
}

bool array_write_to_jpeg( const uchar* refarray, int w, int h, int d, const char* refpath, int q )
{
    if ( ( refarray == NULL ) || ( w <= 0 ) || ( h <= 0 ) || ( d != 3 ) )
        return false;

    // write JPEG.
    if ( access( refpath, F_OK ) == 0 )
    {
        if ( remove(refpath) != 0 )
        {
            return false;
        }
    }

    FILE* fp = fopen( refpath , "wb+" );

    if ( fp == NULL )
    {
        return false;
    }

    struct jpeg_compress_struct cinfo = {0};
    struct jpeg_error_mgr jerr = {0};
    JSAMPROW row_pointer[1] = {0};

    cinfo.err=  jpeg_std_error( &jerr );

    jpeg_create_compress( &cinfo );
    jpeg_stdio_dest( &cinfo, fp );

    cinfo.image_width = w;
    cinfo.image_height = h;
    cinfo.input_components = d;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults( &cinfo );
    cinfo.image_width  = w;
    cinfo.image_height = h;
    cinfo.input_components = d;
    cinfo.in_color_space = JCS_RGB;
    cinfo.dct_method = JDCT_FLOAT;

    float qty = float( q ) / 100;
    if ( qty > 1.0f )
    {
        qty = 1.0f;
    }

    jpeg_set_quality( &cinfo, 256 * qty , 1 );
    jpeg_start_compress( &cinfo, 1 );

    while( cinfo.next_scanline < cinfo.image_height )
    {
        row_pointer[0] = (uchar*)&refarray[ cinfo.next_scanline * cinfo.image_width * cinfo.input_components ];
        jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }

    jpeg_finish_compress( &cinfo );
    jpeg_destroy_compress( &cinfo );
    fclose( fp );

    return true;
}
