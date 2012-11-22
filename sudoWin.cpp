// sudoWin.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sudoWin.h"

#include <fcntl.h>
#include <io.h>
#include <iostream>

#include "installer.h"
#include "params.h"

using namespace std;

int NewCmd( LPTSTR lpCmdLine );

int ExecuteCmd();

int _tmain(int argc, _TCHAR* argv[])
{
    CString CommandLine;
    bool fAddQuotes = false;

    for (int i = 0; i < argc; i++)
    {
        if (_tcschr(argv[i], _T(' ')))
        {
            fAddQuotes = true;
            CommandLine += _T('\"');
        }

        CommandLine += argv[i];

        if (fAddQuotes)
            CommandLine += _T('\"');

        CommandLine += _T(' ');
    }

    return _tWinMain(0, 0, CommandLine.GetBuffer(), 0);
}


int APIENTRY _tWinMain(HINSTANCE /*hInstance*/,
    HINSTANCE /*hPrevInstance*/,
    LPTSTR    lpCmdLine,
    int       /*nCmdShow*/)
{
    if (( lpCmdLine ) && ( lpCmdLine[0] ))
	{
		if ( lpCmdLine[0] != '/')
			return NewCmd( lpCmdLine );
		else 
			return ExecuteCmd();
	}

	ComInitializer comObj;

    return Installer::Execute(comObj);
}


int NewCmd(LPTSTR lpCmdLine)
{
	Params::Save(lpCmdLine);

	DWORD dwProcessId = ::GetCurrentProcessId();
    CString strEvent;
    strEvent.Format(c_szEventRun, dwProcessId);
    CHandle hEventRun( ::CreateEvent( NULL, TRUE, FALSE, strEvent));
	::ResetEvent( hEventRun);

	strEvent.Format(c_szEventExit, dwProcessId);
	CHandle hEventExit( ::CreateEvent( NULL, TRUE, FALSE, strEvent));
	::ResetEvent( hEventExit);

    CAtlString sApp = _T("schtasks /run /TN \"") TASK_NAME _T("\"");

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

int ExecuteCmd()
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

	CString strEvent;
    strEvent.Format(c_szEventRun, dwProcessId);
    
	CHandle hEventRun (::OpenEvent( EVENT_MODIFY_STATE, FALSE, strEvent));
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

    strEvent.Format(c_szEventExit, dwProcessId);
    
	CHandle hEventExit (::OpenEvent( EVENT_MODIFY_STATE, FALSE, strEvent));
	if (hEventExit.m_h)
    {
        ::SetEvent(hEventExit);
    }

	Params::Clear();
    
    return ecNoError;
}

