#include "stdafx.h"

#include "installer.h"
#include "resource.h"

Installer::InstallStatus Installer::Execute(ComInitializer &comObject)
{
	InstallStatus result = isUnknown;

	Installer inst(comObject);
	BOOL bIsInstalled = inst.IsTaskExist();
	try
	{
		if (bIsInstalled)
		{
			inst.ExecuteUninstall();
			result = InstallStatus::isUninstalled;
		}
		else
		{
			inst.ExecuteInstall();
			result = InstallStatus::isInstalled;
		}
	}
	catch (long code)
	{
		result = ProcessError(code, bIsInstalled);
	}
	return result;
}

Installer::InstallStatus Installer::Install(ComInitializer &comObject)
{
	InstallStatus result;

	Installer inst(comObject);
	if (inst.IsTaskExist())
	{
		ProcessError( ERROR_INVALID_FUNCTION, true);
		result = InstallStatus::isAlready;
	}
	else
	{
		try
		{
			inst.ExecuteInstall();
			result = InstallStatus::isInstalled;
		}
		catch (long code)
		{
			result = ProcessError(code, false);
		}
	}

	return result;
}

Installer::InstallStatus Installer::Uninstall(ComInitializer &comObject)
{
	InstallStatus result;

	Installer inst(comObject);
	if (inst.IsTaskExist() == FALSE)
	{
		ProcessError( ERROR_INVALID_FUNCTION, false);
		result = isAlready;
	}
	else
	{
		try
		{
			inst.ExecuteUninstall();
			result = InstallStatus::isUninstalled;
		}
		catch (long code)
		{
			result = ProcessError(code, true);
		}

	}

	return result;
}

Installer::InstallStatus Installer::ProcessError( int code, BOOL isInstalled)
{
	Installer::InstallStatus result = InstallStatus::isUnknown;
	CAtlString message;
	CAtlString title;
	switch (code)
	{
	case 1:
		title.LoadString(IDS_TITLE_INSTALATION);
		message.LoadString(IDS_ALREADY);
		::MessageBox( NULL, message, title, MB_ICONINFORMATION );
		break;
	case 5:	
		title.LoadString(IDS_TITLE_INSTALATION);
		if (isInstalled)
			message.LoadString(IDS_UNINSTALL_USAGE_INFO);
		else
			message.LoadString(IDS_INSTALATION_INFO);
		::MessageBox( NULL, message, title, MB_ICONINFORMATION );
		result = InstallStatus::isPrivileges;
		break;
	default:
		result = InstallStatus::isUnknown;
		break;
	}
	return result;
}

void Installer::ThrowOnError()
{
	m_comObj.ThrowOnError(m_hr);
}

Installer::Installer(ComInitializer &com):m_comObj(com)
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
	m_pFolder->GetTask(_bstr_t(TASK_NAME), &pTask);
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

	CAtlString message;
	CAtlString title;
	title.LoadString(IDS_TITLE_INSTALATION);
	CAtlString platform;
#ifdef _WIN64
	platform.LoadString(IDS_X64);
#else
	platform.LoadString(IDS_X86);
#endif
	message.Format(IDS_INSTALLED, platform);
	::MessageBox( NULL, message, title, MB_ICONINFORMATION );
}

void Installer::ExecuteUninstall()
{
	InstallStatus result = isUnknown;

	m_hr = m_pFolder->DeleteTask(_bstr_t(TASK_NAME), 0);
	ThrowOnError();

	CAtlString message;
	CAtlString title;
	title.LoadString(IDS_TITLE_INSTALATION);
	CAtlString platform;
#ifdef _WIN64
	platform.LoadString(IDS_X64);
#else
	platform.LoadString(IDS_X86);
#endif
	message.Format(IDS_UNINSTALLED, platform);
	::MessageBox( NULL, message, title, MB_ICONINFORMATION );
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

	sSelfPath = "/";
	m_hr = pExecAction->put_Arguments( _bstr_t(sSelfPath.GetString()) );
	ThrowOnError();

	// Register
	CComPtr<IRegisteredTask> pRegisteredTask;
	m_hr = m_pFolder->RegisterTaskDefinition( _bstr_t(TASK_NAME), pDefinition, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pRegisteredTask);
	ThrowOnError();
}
