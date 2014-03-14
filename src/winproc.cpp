#include <windows.h>
#include "winproc.h"

typedef struct _ProcessCallbackData
{
    DWORD m_pid;
    HWND  m_hwnd;
} ProcessCallbackData;


BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lParam)
{
    DWORD pid = 0;
    ProcessCallbackData *pcbd = (ProcessCallbackData *)lParam;

    if ( IsWindowVisible(hwnd) == FALSE )
        return TRUE;

    GetWindowThreadProcessId(hwnd, &pid);

    if ( pcbd->m_pid != pid )
        return TRUE;

    pcbd->m_hwnd = hwnd;

    return FALSE;
}

HWND GetHandleFromPID(DWORD pid)
{
    ProcessCallbackData cbd = {0};

    if (pid == 0)
        return NULL;

    cbd.m_pid = pid;
    cbd.m_hwnd = NULL;

    EnumWindows( EnumProc, (LPARAM)&cbd );

    return cbd.m_hwnd;
}
