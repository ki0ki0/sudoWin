// sudoWin.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sudoWin.h"

#include <fcntl.h>
#include <io.h>
#include <iostream>

#include "installer.h"

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

    return Installer::Execute();
}


int NewCmd(LPTSTR lpCmdLine)
{
    LPTSTR lpArgs = PathGetArgs( lpCmdLine );
    PathRemoveArgs( lpCmdLine );

    CAtlString sDir;

    if (( lpCmdLine == NULL ) || ( lpCmdLine[0] == 0 ))
        return ecUnknown;

    DWORD dwLen = 0;
    dwLen = GetCurrentDirectory( dwLen, NULL );
    sDir.GetBuffer( dwLen );
    dwLen = GetCurrentDirectory( dwLen, sDir.GetBuffer( dwLen ));
    sDir.ReleaseBuffer( dwLen );

    LSTATUS lStatus;
    lStatus = ::RegSetKeyValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecApp, REG_SZ, lpCmdLine, ( _tcslen(lpCmdLine) + 1 ) * sizeof( lpCmdLine[0] ));
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
        return ecRegSet;

    lStatus = ::RegSetKeyValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecArgs, REG_SZ, lpArgs, ( _tcslen(lpArgs) + 1 ) * sizeof( lpArgs[0] ));
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
        return ecRegSet;

    lStatus = ::RegSetKeyValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecDir, REG_SZ, sDir.GetString(), ( sDir.GetLength() + 1 ) * sizeof( sDir[0] ));
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
        return ecRegSet;

    AttachConsole(ATTACH_PARENT_PROCESS);

    DWORD dwProcessId = ::GetCurrentProcessId();

    lStatus = ::RegSetKeyValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecId, REG_BINARY, &dwProcessId, sizeof(dwProcessId));
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
        return ecRegSet;

    CString strEvent;
    strEvent.Format(_T("sudoWin%d"), dwProcessId);
    
    CHandle hEvent( ::CreateEvent( NULL, TRUE, FALSE, strEvent));

	// todo: add "runned" event, and timeout for it

    ::ResetEvent( hEvent);

    CAtlString sApp = _T("schtasks /run /TN \"") TASK_NAME _T("\"");

    STARTUPINFO startup_info = {0};
    startup_info.cb = sizeof( STARTUPINFO );
    PROCESS_INFORMATION process_info = {0};

    BOOL bStatus = ::CreateProcess( NULL, sApp.GetBuffer(MAX_PATH), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info );
    ATLASSERT( SUCCEEDED( bStatus ));
    if ( FAILED( bStatus ))
        return ecRunTask;
    ::CloseHandle( process_info.hProcess );
    ::CloseHandle( process_info.hThread );

    ::WaitForSingleObject(hEvent, INFINITE);

    return ecNoError;
}

int ExecuteCmd()
{
    DWORD dwType;
    DWORD dwLen = 0;

    LSTATUS lStatus;
    lStatus = ::RegGetValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecApp, RRF_RT_REG_SZ, &dwType, NULL, &dwLen );
    if ( lStatus != ERROR_SUCCESS )
    {
        return Installer::Install();
    }

    CAtlString sApp;
    sApp.GetBuffer( dwLen / sizeof(TCHAR) + 1 );
    lStatus = ::RegGetValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecApp, RRF_RT_REG_SZ, &dwType, sApp.GetBuffer(), &dwLen );
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
    {
        return Installer::Install();
    }
    sApp.ReleaseBuffer();

    if ( !sApp[0] )
    {
		return Installer::Install();
    }

    lStatus = ::RegGetValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecArgs, RRF_RT_REG_SZ, &dwType, NULL, &dwLen );
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
        return ecRegGet;

    CAtlString sArgs;
    sArgs.GetBuffer( dwLen / sizeof(TCHAR) + 1 );
    lStatus = ::RegGetValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecArgs, RRF_RT_REG_SZ, &dwType, sArgs.GetBuffer(), &dwLen );
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
        return ecRegGet;
    sArgs.ReleaseBuffer();

    lStatus = ::RegGetValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecDir, RRF_RT_REG_SZ, &dwType, NULL, &dwLen );
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
        return ecRegGet;

    CAtlString sDir;
    sDir.GetBuffer( dwLen / sizeof(TCHAR) + 1 );
    lStatus = ::RegGetValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecDir, RRF_RT_REG_SZ, &dwType, sDir.GetBuffer(), &dwLen );
    ATLASSERT( lStatus == ERROR_SUCCESS );
    if ( lStatus != ERROR_SUCCESS )
        return ecRegGet;
    sDir.ReleaseBuffer();

    DWORD dwProcessId = 0;
    dwLen = sizeof(dwProcessId);
    lStatus = ::RegGetValue( HKEY_CURRENT_USER, c_szRegPath, c_szExecId, RRF_RT_REG_BINARY, &dwType, &dwProcessId, &dwLen );

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

    BOOL bStatus = ::CreateProcess( NULL, sApp.GetBuffer(MAX_PATH), NULL, NULL, TRUE, 0, NULL, lpDir, &startup_info, &process_info );
    ATLASSERT( SUCCEEDED( bStatus ));
    if ( FAILED( bStatus ))
        return ecRegSHExec;

    if (process_info.hProcess != NULL)
        ::WaitForSingleObject(process_info.hProcess, INFINITE);

    ::CloseHandle( process_info.hProcess );
    ::CloseHandle( process_info.hThread );

    if (bConsole)
	    cout << endl;

    CString strEvent;
    strEvent.Format(_T("sudoWin%d"), dwProcessId);
    
    HANDLE hEvent = ::OpenEvent( EVENT_MODIFY_STATE, FALSE, strEvent);
    if (hEvent)
    {
        ::SetEvent(hEvent);
        ::CloseHandle(hEvent);
    }

    NewCmd( _T("") );

    return ecNoError;
}

