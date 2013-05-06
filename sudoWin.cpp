// sudoWin.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sudoWin.h"

#include "installer.h"
#include "Runner.h"
#include "win32_exception.h"

#ifdef _WIN64
#define TASK_NAME	_T("sudoWin64")
#else
#define TASK_NAME	_T("sudoWin")
#endif

#define TASK_EXECUTION_PARAMETER	_T("/execute")

BOOL CheckCmdParam(CAtlString str, LPTSTR param);
DWORD ProcessError( HRESULT hrStatus, BOOL bInstall, LPCSTR szMessage, BOOL bSilent);
void ShowMessage(LPCTSTR szMessage);

#define Message(x) if (bSilent == FALSE) ShowMessage(x)

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
	HRESULT hrStatus = S_OK;
	BOOL bInstall = TRUE;
	BOOL bSilent = FALSE;
	try
	{
		if ((lpCmdLine == NULL) || (lpCmdLine[0] == 0))
		{
			ComInitializer comObj;
			hrStatus = Installer::Execute(TASK_NAME, TASK_EXECUTION_PARAMETER, bInstall, comObj);
			CAtlString str;
			str.LoadString(bInstall? IDS_INSTALLED : IDS_UNINSTALLED);
			Message(str);
		}
		else
		{
			CAtlString str(lpCmdLine);

			bSilent = CheckCmdParam(str, _T("/s"));

			if (CheckCmdParam(str, _T("/i")))
			{
				bInstall = TRUE;
				ComInitializer comObj;
				hrStatus = Installer::Install(TASK_NAME, TASK_EXECUTION_PARAMETER, comObj);
				CAtlString str;
				str.LoadString(IDS_INSTALLED);
				Message(str);
			}
			else if (CheckCmdParam(str, _T("/u")))
			{
				bInstall = FALSE;
				ComInitializer comObj;
				hrStatus = Installer::Uninstall(TASK_NAME, comObj);
				CAtlString str;
				str.LoadString(IDS_UNINSTALLED);
				Message(str);
			}
			else if (CheckCmdParam(str, TASK_EXECUTION_PARAMETER))
			{
				hrStatus = Runner::ExecuteCmd();
			}
			else
			{
				try
				{
					hrStatus = Runner::NewCmd(TASK_NAME, lpCmdLine );
				}
				catch (std::invalid_argument)
				{
					ComInitializer comObj;
					hrStatus = Installer::Execute(TASK_NAME, TASK_EXECUTION_PARAMETER, bInstall, comObj);
					CAtlString str;
					str.LoadString(bInstall? IDS_INSTALLED : IDS_UNINSTALLED);
					Message(str);
				}
			}
		}
	}
	catch (win32_exception ex)
	{
		return ProcessError(ex.GetStatus(), bInstall, ex.what(), bSilent);
	}
	return HRESULT_CODE(hrStatus);
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


DWORD ProcessError( HRESULT hrStatus, BOOL bInstall, LPCSTR szMessage, BOOL bSilent)
{
	CAtlString cmd(::GetCommandLine()); 
	::PathRemoveArgs(cmd.GetBuffer());
	cmd.ReleaseBuffer();
	CAtlString name = ::PathFindFileName(cmd);
	name.Replace(_T('\"'),_T(''));

	CAtlString message;
	
	switch (HRESULT_CODE(hrStatus))
	{
	case 1:
		message.LoadString(IDS_ALREADY);
		break;
	case 5:	
		if (bInstall)
			message.LoadString(IDS_INSTALATION_INFO);
		else
			message.LoadString(IDS_UNINSTALL_USAGE_INFO);
		message.Replace(_T("%sudoWin%"), name);
		break;
	default:
		message.Format(IDS_UNKNOWN, hrStatus);
		break;
	}
	Message(message);
	return HRESULT_CODE(hrStatus);
}

void ShowMessage(LPCTSTR szMessage)
{
	CAtlString strPlatform;

#ifdef _WIN64
		strPlatform.LoadString(IDS_X64);
#else
		strPlatform.LoadString(IDS_X86);
#endif

	CAtlString strTitle;
	strTitle.LoadString(IDS_TITLE_INSTALATION);

	CAtlString strMessage;
	strMessage.Format(szMessage, strPlatform);

	if (::AttachConsole(-1) == FALSE)
		::MessageBox( NULL, strMessage, strTitle, MB_ICONINFORMATION );
	else
	{
		std::wstreambuf *backup = std::wcout.rdbuf();
		std::wofstream console_out("CONOUT$");
		std::wcout.rdbuf(console_out.rdbuf());

		std::wcout<<std::endl<<strTitle<<std::endl<<szMessage<<std::endl<<std::endl;

		console_out.close();
		std::wcout.rdbuf(backup);
	}
}