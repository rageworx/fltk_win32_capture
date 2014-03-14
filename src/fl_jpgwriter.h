#ifndef __FL_JPGWRITER_H__
#define __FL_JPGWRITER_H__

#include <FL/Fl.H>
#include <FL/Fl_Image.H>

bool fl_image_write_to_jpeg( Fl_Image* refimg, const char* refpath, int q );
bool array_write_to_jpeg( const uchar* refarray, int w, int h, int d, const char* refpath, int q );

#endif /// of __FL_JPGWRITER_H__
