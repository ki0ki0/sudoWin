// sudoWin.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sudoWin.h"

#include "installer.h"
#include "Runner.h"
#include "win32_exception.h"
#include "CmdLineParser.h"

#ifdef _WIN64
#define TASK_NAME	_T("sudoWin64")
#else
#define TASK_NAME	_T("sudoWin")
#endif

#define TASK_EXECUTION_PARAMETER	_T("/execute")

DWORD ProcessError(DWORD dwMode, HRESULT hrStatus, LPCSTR szMessage = NULL);
void ShowMessage(LPCTSTR szMessage);

enum Mode
{
	mNone = 0,
	mInstall = 1,
	mUninstall = 2,
	mRun = 4,
	mSilence = 8
};

int APIENTRY _tWinMain(HINSTANCE /*hInstance*/,
    HINSTANCE /*hPrevInstance*/,
    LPTSTR    lpCmdLine,
    int       /*nCmdShow*/)
{
	HRESULT hrStatus = S_OK;
	CCmdLineParser cmdParser(lpCmdLine);
	cmdParser.AddFlag(_T("/i"), mInstall);
	cmdParser.AddFlag(_T("/u"), mUninstall);
	cmdParser.AddFlag(TASK_EXECUTION_PARAMETER, mRun);
	DWORD dwMode = cmdParser.AddFlag(_T("/s"), mSilence);

	try
	{
		if ((dwMode & mInstall) == mInstall)
		{
			ComInitializer comObj;
			hrStatus = Installer::Install(TASK_NAME, TASK_EXECUTION_PARAMETER, comObj);
		}
		else if ((dwMode & mUninstall) == mUninstall)
		{
			ComInitializer comObj;
			hrStatus = Installer::Uninstall(TASK_NAME, comObj);
		}
		else if ((dwMode & mRun) == mRun)
		{
			hrStatus = Runner::ExecuteCmd();
		}
		else if ((lpCmdLine == NULL) || (lpCmdLine[0] == 0) || (dwMode == mSilence))
		{
			BOOL bInstall = FALSE;
			ComInitializer comObj;
			try
			{
				hrStatus = Installer::Execute(TASK_NAME, TASK_EXECUTION_PARAMETER, bInstall, comObj);
			}
			catch(...)
			{
				dwMode |= bInstall? mInstall : mUninstall;
				throw;
			}
			dwMode |= bInstall? mInstall : mUninstall;
		}
		else 
		{
			hrStatus = Runner::NewCmd(TASK_NAME, lpCmdLine );
		}
		
	}
	catch (win32_exception ex)
	{
		return ProcessError(dwMode, ex.GetStatus(), ex.what());
	}
	return ProcessError(dwMode, hrStatus);
}

DWORD ProcessError(DWORD dwMode, HRESULT hrStatus, LPCSTR szMessage)
{
	CAtlString cmd(::GetCommandLine()); 
	::PathRemoveArgs(cmd.GetBuffer());
	cmd.ReleaseBuffer();
	CAtlString name = ::PathFindFileName(cmd);
	name.Replace(_T('\"'),_T(''));

	CAtlString message;
	
	switch (HRESULT_CODE(hrStatus))
	{
	case 0:
		if ((dwMode & mInstall) == mInstall)
			message.LoadString(IDS_INSTALLED);
		else if ((dwMode & mUninstall) == mUninstall)
			message.LoadString(IDS_UNINSTALLED);
		break;
	case 1:
		message.LoadString(IDS_ALREADY);
		break;
	case 5:	
		if ((dwMode & mInstall) == mInstall)
			message.LoadString(IDS_INSTALATION_INFO);
		else if ((dwMode & mUninstall) == mUninstall)
			message.LoadString(IDS_UNINSTALL_USAGE_INFO);
		else 
			message.LoadString(IDS_UNKNOWN);
		break;
	default:
		message.LoadString(IDS_UNKNOWN);
		break;
	}
	if ((dwMode & mSilence) != mSilence)
	{
		if (FAILED(hrStatus))
		{
			CAtlString strError;
			strError.Format(IDS_ERROR_MESSAGE, hrStatus, szMessage);
			message += _T("\n"); 
			message += strError;
		}
		message.Replace(_T("%sudoWin%"), name);
		ShowMessage(message);
	}
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