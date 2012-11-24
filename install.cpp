#include "stdafx.h"

#include "installer.h"
#include "resource.h"

Installer::InstallStatus Installer::Execute(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &comObject, BOOL bSilent)
{
	long errorCode = ERROR_SUCCESS;

	Installer inst(taskname, taskparam, comObject);
	BOOL bIsInstalled = inst.IsTaskExist();
	try
	{
		if (bIsInstalled)
		{
			inst.ExecuteUninstall();
		}
		else
		{
			inst.ExecuteInstall();
		}
		errorCode = ERROR_SUCCESS;
	}
	catch (long code)
	{
		errorCode = code;
	}
	return ProcessError(errorCode, bIsInstalled, bSilent);
}

Installer::InstallStatus Installer::Install(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &comObject, BOOL bSilent)
{
	long errorCode = ERROR_SUCCESS;

	Installer inst(taskname, taskparam, comObject);
	BOOL bIsInstalled = inst.IsTaskExist();
	if (bIsInstalled)
	{
		errorCode = ERROR_INVALID_FUNCTION;
	}
	else
	{
		try
		{
			inst.ExecuteInstall();
			errorCode = ERROR_SUCCESS;
		}
		catch (long code)
		{
			errorCode = code;
		}
	}

	return ProcessError(errorCode, bIsInstalled, bSilent);
}

Installer::InstallStatus Installer::Uninstall(LPCTSTR taskname, ComInitializer &comObject, BOOL bSilent)
{
	long errorCode = ERROR_SUCCESS;

	Installer inst(taskname, nullptr, comObject);
	BOOL bIsInstalled = inst.IsTaskExist();
	if (bIsInstalled == FALSE)
	{
		errorCode = ERROR_INVALID_FUNCTION;
	}
	else
	{
		try
		{
			inst.ExecuteUninstall();
			errorCode = ERROR_SUCCESS;
		}
		catch (long code)
		{
			errorCode = code;
		}

	}

	return ProcessError(errorCode, bIsInstalled, bSilent);
}

Installer::InstallStatus Installer::ProcessError( int code, BOOL isInstalled, BOOL bSilent)
{
	Installer::InstallStatus result = InstallStatus::isUnknown;

	CAtlString cmd(::GetCommandLine()); 
	::PathRemoveArgs(cmd.GetBuffer());
	cmd.ReleaseBuffer();
	CAtlString name = ::PathFindFileName(cmd);
	name.Replace(_T('\"'),_T(''));

	CAtlString message;
	CAtlString title;
	title.LoadString(IDS_TITLE_INSTALATION);
	CAtlString platform;
	
	switch (code)
	{
	case 0:
#ifdef _WIN64
		platform.LoadString(IDS_X64);
#else
		platform.LoadString(IDS_X86);
#endif
		if (isInstalled)
		{
			message.Format(IDS_UNINSTALLED, platform);
			result = InstallStatus::isUninstalled;
		}
		else
		{
			message.Format(IDS_INSTALLED, platform);
			result = InstallStatus::isInstalled;
		}
		break;
	case 1:
		message.LoadString(IDS_ALREADY);
		result = isAlready;
		break;
	case 5:	
		if (isInstalled)
			message.LoadString(IDS_UNINSTALL_USAGE_INFO);
		else
			message.LoadString(IDS_INSTALATION_INFO);
		message.Replace(_T("%sudoWin%"), name);
		result = InstallStatus::isPrivileges;
		break;
	default:
		message.Format(IDS_UNKNOWN, code);
		result = InstallStatus::isUnknown;
		break;
	}
	if (bSilent == FALSE)
	{
		if (::AttachConsole(-1) == FALSE)
			::MessageBox( NULL, message, title, MB_ICONINFORMATION );
		else
		{
			std::wstreambuf *backup = std::wcout.rdbuf();
			std::wofstream console_out("CONOUT$");
			std::wcout.rdbuf(console_out.rdbuf());

			std::wcout<<std::endl<<title.GetString()<<std::endl<<message.GetString()<<std::endl<<std::endl;

			console_out.close();
			std::wcout.rdbuf(backup);
		}
	}
	return result;
}

void Installer::ThrowOnError()
{
	m_comObj.ThrowOnError(m_hr);
}

Installer::Installer(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &com):
	m_comObj(com), m_lpTaskName(taskname), m_lpTaskParam(taskparam)
{
	m_hr = CoCreateInstance( CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&m_pService );  
	ThrowOnError();

	m_hr = m_pService->Connect( _variant_t(), _variant_t(), _variant_t(), _variant_t());
	ThrowOnError();

	m_hr = m_pService->GetFolder(_T("\\"), &m_pFolder);
	ThrowOnError();
}

BOOL Installer::IsTaskExist()
{
	CComPtr<IRegisteredTask> pTask;
	m_pFolder->GetTask(_bstr_t(m_lpTaskName), &pTask);
	ThrowOnError();
	return (pTask != NULL);
}

void Installer::ExecuteInstall()
{
	DWORD dwLen = MAX_PATH;
	CAtlString sSelfPath;
	DWORD dwRes;
	do
	{
		dwRes = ::GetModuleFileName( NULL, sSelfPath.GetBuffer(dwLen), dwLen);
	} 
	while ((dwRes == dwLen) && (::GetLastError() == ERROR_INSUFFICIENT_BUFFER));

	CreateTask( sSelfPath );
}

void Installer::ExecuteUninstall()
{
	InstallStatus result = isUnknown;

	m_hr = m_pFolder->DeleteTask(_bstr_t(m_lpTaskName), 0);
	ThrowOnError();
}

void Installer::CreateTask(CAtlString &sSelfPath)
{
	CComPtr<ITaskDefinition> pDefinition;
	m_hr = m_pService->NewTask(0, &pDefinition);
	ThrowOnError();

	//TaskSettings
	CComPtr<ITaskSettings> pTaskSettings;
	m_hr = pDefinition->get_Settings(&pTaskSettings);
	ThrowOnError();

	m_hr = pTaskSettings->put_DisallowStartIfOnBatteries(FALSE);
	ThrowOnError();

	m_hr = pTaskSettings->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
	ThrowOnError();

	//Principal
	CComPtr<IPrincipal> pPrincipal;
	m_hr = pDefinition->get_Principal(&pPrincipal);
	ThrowOnError();

	m_hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	ThrowOnError();

	//IActionCollection
	CComPtr<IActionCollection> pActionCollection;
	m_hr = pDefinition->get_Actions( &pActionCollection );
	ThrowOnError();

	CComPtr<IAction> pAction;
	m_hr = pActionCollection->Create( TASK_ACTION_EXEC, &pAction );
	ThrowOnError();

	CComPtr<IExecAction> pExecAction;
	m_hr = pAction->QueryInterface( IID_IExecAction, (void**)&pExecAction );
	ThrowOnError();

	m_hr = pExecAction->put_Path( _bstr_t(sSelfPath.GetString()) );
	ThrowOnError();

	m_hr = pExecAction->put_Arguments( _bstr_t(m_lpTaskParam) );
	ThrowOnError();

	// Register
	CComPtr<IRegisteredTask> pRegisteredTask;
	m_hr = m_pFolder->RegisterTaskDefinition( _bstr_t(m_lpTaskName), pDefinition, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pRegisteredTask);
	ThrowOnError();
}
