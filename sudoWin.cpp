// sudoWin.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sudoWin.h"

#include "installer.h"
#include "Runner.h"

#ifdef _WIN64
#define TASK_NAME	_T("sudoWin64")
#else
#define TASK_NAME	_T("sudoWin")
#endif

#define TASK_EXECUTION_PARAMETER	_T("/execute")

BOOL CheckCmdParam(CAtlString str, LPTSTR param);

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
	if (lpCmdLine == NULL)
	{
		ComInitializer comObj;
		return Installer::Execute(TASK_NAME, TASK_EXECUTION_PARAMETER, comObj);
	}

	BOOL bSilent = FALSE;
	CAtlString str(lpCmdLine);
	if (CheckCmdParam(str, _T("/s")))
	{
		bSilent = TRUE;
	}

	if (CheckCmdParam(str, _T("/i")))
	{
		ComInitializer comObj;
		return Installer::Install(TASK_NAME, TASK_EXECUTION_PARAMETER, comObj, bSilent);
	}

	if (CheckCmdParam(str, _T("/u")))
	{
		ComInitializer comObj;
		return Installer::Uninstall(TASK_NAME, comObj, bSilent);
	}

	if (bSilent)
	{
		ComInitializer comObj;
		return Installer::Execute(TASK_NAME, TASK_EXECUTION_PARAMETER, comObj, bSilent);
	}

	if (CheckCmdParam(str, TASK_EXECUTION_PARAMETER))
	{
		return Runner::ExecuteCmd();
	}
	return Runner::NewCmd(TASK_NAME, lpCmdLine );
}

BOOL CheckCmdParam(CAtlString str, LPTSTR param)
{
	int pos = str.Find(param);
	if (pos == -1)
		return FALSE;
	
	size_t len = _tcslen(param);
	if (pos+len == str.GetLength())
		return TRUE;
	
	if (str[pos+len] == ' ')
		return TRUE;

	return FALSE;
}
