#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <io.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Image.H>

#include "winproc.h"
#include "winbmp.h"
#include "fl_jpgwriter.h"

Fl_Window* window = NULL;
Fl_Button* btn_capture_test = NULL;
Fl_Box*    box_dsp = NULL;

void flw_cb( Fl_Widget* w, void* p )
{
    btn_capture_test->deactivate();

	STARTUPINFO         SI = {0};
	PROCESS_INFORMATION PI = {0};

 	SI.cb = sizeof(STARTUPINFO);

 	char runa[] = "notepad.exe";

    BOOL retB = CreateProcess( NULL, runa, NULL, NULL, FALSE, 0, NULL, NULL, &SI, &PI );

    if ( retB == TRUE )
    {
        // Wait for Loading (example, not for release.)
        WaitForSingleObject( PI.hProcess, 500 );

        // Get Window handle by process ID.
        HWND hWnd = GetHandleFromPID( PI.dwProcessId );
        if ( hWnd != NULL )
        {
            // Make found window improve order to top.
            SetForegroundWindow( hWnd );

            // Get DC from found window.
            HDC hDC = GetWindowDC( hWnd );
            RECT winR = {0};
            GetWindowRect( hWnd, &winR );

            int nW = winR.right - winR.left;
            int nH = winR.bottom - winR.top;

            HBITMAP hBmp = HDC2HBITMAP( hDC, nW, nH );
            if ( hBmp != NULL )
            {
                PBITMAPINFO bitmapInfo = CreateBitmapInfoStruct( hBmp );
#ifdef IMAGE_WRITE2BMP_W_WINAPI
                // In case of write BMP file by using Windows API.
                CreateBMPFile( "test.bmp", bitmapInfo, hBmp, hDC );
#else

                // It will make 24bit RGB Fl_Image.
                uchar* bmpdata = new uchar[ nW * nH * 4];
                uchar* flidata = new uchar[ nW * nH * 3];
                if ( ( bmpdata != NULL ) && ( flidata != NULL ) )
                {
                    GetDIBits( hDC, hBmp, 0, nH, bmpdata, bitmapInfo, DIB_RGB_COLORS );

                    // Let it delete after done to use it.
                    DeleteObject( hBmp );
                    DeleteObject( hDC );

                    // Make Windows DIB arrays to RGB.
                    for(int cy=0;cy<nH; cy++)
                    {
                        for(int cx=0;cx<nW; cx++)
                        {
                            uchar* cur_b_pixel = &bmpdata[ ( ( ( nH - cy - 1) * nW ) + cx ) * 4 ];
                            uchar* cur_f_pixel = &flidata[ ( ( cy * nW ) + cx ) * 3 ];

                            cur_f_pixel[0] = cur_b_pixel[2];
                            cur_f_pixel[1] = cur_b_pixel[1];
                            cur_f_pixel[2] = cur_b_pixel[0];
                        }
                    }

                    delete[] bmpdata;

                    array_write_to_jpeg( flidata, nW, nH, 3, "test.jpg", 81 );

                    Fl_RGB_Image* convimg = new Fl_RGB_Image( flidata, nW, nH, 3);
                    if ( convimg != NULL )
                    {
                        box_dsp->image( convimg->copy( box_dsp->w(), box_dsp->h() ) );
                        box_dsp->redraw();
                    }

                    delete convimg;
                }
#endif
            }
        }

        DWORD eCode = 0;
        TerminateProcess( PI.hProcess, eCode );
    }
    else
    {
        MessageBox(0,"Failed to run a process !","Error",MB_OK);
    }

    btn_capture_test->activate();

}

////////////////////////////////////////////////////////////////////////////////

int main (int argc, char ** argv)
{
    window = new Fl_Window (600, 400);
    if ( window != NULL )
    {
        window->begin();

        btn_capture_test = new Fl_Button(0,0,600,20, "Capture test !");
        if ( btn_capture_test != NULL )
        {
            btn_capture_test->callback( flw_cb, NULL );
        }

        box_dsp = new Fl_Box(0,20,600,380);
        if ( box_dsp != NULL )
        {
            box_dsp->box( FL_NO_BOX );
            box_dsp->align( FL_ALIGN_CLIP );
        }

        window->end();
        window->show();
    }

    return(Fl::run());
}
