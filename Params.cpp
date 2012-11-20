#include "stdafx.h"

#include "params.h"
#include "regvalue.h"

void Params::Save( LPTSTR lpCmdLine)
{
	if (( lpCmdLine == NULL ))
        throw "Invalid argument";

	LPTSTR lpArgs = PathGetArgs( lpCmdLine );
    PathRemoveArgs( lpCmdLine );

    CAtlString sDir;
	DWORD dwLen = 0;
    dwLen = GetCurrentDirectory( dwLen, NULL );
    sDir.GetBuffer( dwLen );
    dwLen = GetCurrentDirectory( dwLen, sDir.GetBuffer( dwLen ));
    sDir.ReleaseBuffer( dwLen );

	RegValue reg(HKEY_CURRENT_USER, c_szRegPath);

	reg.SetString( c_szExecApp, lpCmdLine, ( _tcslen(lpCmdLine) + 1 ) * sizeof( lpCmdLine[0] ));
    reg.SetString( c_szExecArgs, lpArgs, ( _tcslen(lpArgs) + 1 ) * sizeof( lpArgs[0] ));
    reg.SetString( c_szExecDir, sDir.GetString(), ( sDir.GetLength() + 1 ) * sizeof( sDir[0] ));

    AttachConsole(ATTACH_PARENT_PROCESS);
    DWORD dwProcessId = ::GetCurrentProcessId();
	reg.SetDword( c_szExecId, dwProcessId);
}

void Params::Load( CAtlString sDir, CAtlString sApp, CAtlString sArgs, DWORD &dwProcessId)
{
	RegValue reg(HKEY_CURRENT_USER, c_szRegPath);

	sApp = reg.GetString( c_szExecApp);
	sArgs = reg.GetString( c_szExecArgs);
	sDir = reg.GetString( c_szExecDir);
	dwProcessId = reg.GetDword( c_szExecId);
}

void Params::Clear()
{
	RegValue reg(HKEY_CURRENT_USER, c_szRegPath);

	reg.Delete( c_szExecApp);
    reg.Delete( c_szExecArgs);
    reg.Delete( c_szExecDir);

    reg.Delete( c_szExecId);
}