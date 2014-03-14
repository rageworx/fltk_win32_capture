#ifndef __WINBMP_H__
#define __WINBMP_H__

HBITMAP HDC2HBITMAP( HDC hSrcDC, int nWidth, int nHeight );
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
BOOL CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

#endif /// of __WINBMP_H__
