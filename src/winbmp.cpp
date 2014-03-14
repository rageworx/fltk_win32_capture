#include <windows.h>

HBITMAP HDC2HBITMAP( HDC hSrcDC, int nWidth, int nHeight)
{
    BOOL bSuccess = FALSE;

    HDC hMemDC         = CreateCompatibleDC( hSrcDC );
    HBITMAP hBitmap    = CreateCompatibleBitmap( hSrcDC, nWidth, nHeight );

    if ( hBitmap == NULL )
    {
        return NULL;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    if(!BitBlt(hMemDC, 0, 0, nWidth, nHeight, hSrcDC, 0, 0, SRCCOPY))
        bSuccess = FALSE;
    else
        bSuccess = TRUE;

    SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);

    return hBitmap;
}


PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
    BITMAP      bmp;
    WORD        cClrBits;

    static
    PBITMAPINFO pbmi = {0};

    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
        return NULL;

    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);

    if (cClrBits == 1)
        cClrBits = 1;
    else
    if (cClrBits <= 4)
        cClrBits = 4;
    else
    if (cClrBits <= 8)
        cClrBits = 8;
    else
    if (cClrBits <= 16)
        cClrBits = 16;
    else
    if (cClrBits <= 24)
        cClrBits = 24;
    else
        cClrBits = 32;

    if (cClrBits != 24)
    {

        pbmi = (PBITMAPINFO) LocalAlloc(LPTR,  sizeof(BITMAPINFOHEADER) +
                                        sizeof(RGBQUAD) * (1<< cClrBits));

    }
    else
    {
        pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
    }

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
    pbmi->bmiHeader.biClrUsed = (1<<cClrBits);
    pbmi->bmiHeader.biCompression = BI_RGB;
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8 * pbmi->bmiHeader.biHeight;
    pbmi->bmiHeader.biClrImportant = 0;

    return pbmi;
}

BOOL CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
    HANDLE hf;
    BITMAPFILEHEADER hdr;
    PBITMAPINFOHEADER pbih;
    LPBYTE lpBits;
    DWORD dwTotal;
    DWORD cb;
    BYTE *hp;
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER) pbi;

    if ( pbih->biSizeImage == 0 )
        return FALSE;

    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
        return FALSE;

    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS))
    {
        return FALSE;
    }

    hf = CreateFile( pszFile,
                     GENERIC_READ | GENERIC_WRITE,
                     (DWORD) 0,
                     NULL,
                     CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL,
                     (HANDLE) NULL );

    if (hf == INVALID_HANDLE_VALUE)
        return FALSE;

    hdr.bfType = 0x4d42;
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed
                         * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    hdr.bfOffBits = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed
                            * sizeof (RGBQUAD) );

    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL))
        return FALSE;

    if ( !WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, ( NULL)))
        return FALSE;

    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;

    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))
        return FALSE;

    if (!CloseHandle(hf))
        return FALSE;

    GlobalFree((HGLOBAL)lpBits);

    return TRUE;
}

