#include "stdafx.h"
#include "Runner.h"
#include "params.h"
#include "sudowin.h"

#include <fcntl.h>
#include <io.h>
#include <iostream>

using namespace std;

const LPCTSTR Runner::c_szEventRun = _T("sudoWinRUN");
const LPCTSTR Runner::c_szEventExit = _T("sudoWin%dEXIT");

const DWORD Runner::c_dwRunTimeout = 1000;

int Runner::NewCmd(LPCTSTR taskname, LPTSTR lpCmdLine)
{
	Params::Save(lpCmdLine);

	DWORD dwProcessId = ::GetCurrentProcessId();
    CHandle hEventRun( ::CreateEvent( NULL, FALSE, TRUE, c_szEventRun));
	if (::WaitForSingleObject(hEventRun, c_dwRunTimeout) != WAIT_OBJECT_0)
		return ecSimultanius;

	CString strEvent;
	strEvent.Format(c_szEventExit, dwProcessId);
	CHandle hEventExit( ::CreateEvent( NULL, TRUE, FALSE, strEvent));
	::ResetEvent( hEventExit);

    CAtlString sApp;
	sApp.Format(_T("schtasks /run /TN \"%s\""), taskname);

    STARTUPINFO startup_info = {0};
    startup_info.cb = sizeof( STARTUPINFO );
    PROCESS_INFORMATION process_info = {0};

    BOOL bStatus = ::CreateProcess( NULL, sApp.GetBuffer(MAX_PATH), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info );
    if ( bStatus == FALSE)
        return ecRunTask;
    ::CloseHandle( process_info.hProcess );
    ::CloseHandle( process_info.hThread );

	if (::WaitForSingleObject(hEventRun, c_dwRunTimeout) != WAIT_OBJECT_0)
		return ecUnknown;

    ::WaitForSingleObject(hEventExit, INFINITE);

    return ecNoError;
}

int Runner::ExecuteCmd()
{
    CAtlString sDir, sApp, sArgs;
	DWORD dwProcessId;

	Params::Load( sDir, sApp, sArgs, dwProcessId);

    AttachConsole(dwProcessId);

    HANDLE hDeviceIn = ::GetStdHandle(STD_INPUT_HANDLE);
    SetHandleInformation(hDeviceIn, HANDLE_FLAG_INHERIT, 0);

    HANDLE hDeviceOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetHandleInformation(hDeviceOut, HANDLE_FLAG_INHERIT, 0);

    HANDLE hDeviceErr = GetStdHandle(STD_ERROR_HANDLE);
    SetHandleInformation(hDeviceErr, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFO startup_info = {0};
    startup_info.cb = sizeof( STARTUPINFO );

    BOOL bConsole = FALSE;

    if (hDeviceIn || hDeviceOut || hDeviceErr)
    {
        startup_info.dwFlags = STARTF_USESTDHANDLES;
        startup_info.hStdInput = hDeviceIn;
        startup_info.hStdOutput = hDeviceOut;
        startup_info.hStdError = hDeviceErr;

    	int fd = _open_osfhandle((intptr_t)hDeviceOut, _O_TEXT);
		if (fd > 0)
		{
			*stdout = *_fdopen(fd, "w");
			setvbuf(stdout, NULL, _IONBF, 0 );
        }

	    cout << endl;

        bConsole = TRUE;
    }

    PROCESS_INFORMATION process_info = {0};

    sApp += _T(" ") + sArgs;

    LPCTSTR lpDir = NULL;
    if (sDir.GetLength() > 0)
        lpDir = sDir.GetString();

	CHandle hEventRun (::OpenEvent( EVENT_MODIFY_STATE, FALSE, c_szEventRun));
	if (hEventRun.m_h)
    {
        ::SetEvent(hEventRun);
    }

    BOOL bStatus = ::CreateProcess( NULL, sApp.GetBuffer(MAX_PATH), NULL, NULL, TRUE, 0, NULL, lpDir, &startup_info, &process_info );
    if ( bStatus == FALSE)
        return ecRegSHExec;

    if (process_info.hProcess != NULL)
        ::WaitForSingleObject(process_info.hProcess, INFINITE);

    ::CloseHandle( process_info.hProcess );
    ::CloseHandle( process_info.hThread );

    if (bConsole)
	    cout << endl;

	CString strEvent;
    strEvent.Format(c_szEventExit, dwProcessId);
    
	CHandle hEventExit (::OpenEvent( EVENT_MODIFY_STATE, FALSE, strEvent));
	if (hEventExit.m_h)
    {
        ::SetEvent(hEventExit);
    }

	Params::Clear();
    
    return ecNoError;
}

