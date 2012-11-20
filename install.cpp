#include "stdafx.h"

#include "installer.h"
#include "resource.h"

Installer::InstallStatus Installer::Execute()
{
	InstallStatus result;

	Installer inst;
	BOOL isInstalled = inst.IsTaskExist();
	if (isInstalled)
	{
		result = inst.ExecuteUninstall();
	}
	else
	{
		result = inst.ExecuteInstall();
	}

	return result;
}

Installer::InstallStatus Installer::Install()
{
	InstallStatus result;

	Installer inst;
	if (inst.IsTaskExist())
	{
		result = isAlready;
	}
	else
	{
		result = inst.ExecuteInstall();
	}

	return result;
}

Installer::InstallStatus Installer::Uninstall()
{
	InstallStatus result;

	Installer inst;
	if (inst.IsTaskExist() == FALSE)
	{
		result = isAlready;
	}
	else
	{
		result = inst.ExecuteUninstall();
	}

	return result;
}


void Installer::ThrowOnError()
{
	if (FAILED(m_hr)) // todo: check specific errors
		throw "error";
}

Installer::Installer()
{
	m_hr = ::CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	ThrowOnError();

	m_hr = CoCreateInstance( CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&m_pService );  
	ThrowOnError();

	m_hr = m_pService->Connect( _variant_t(), _variant_t(), _variant_t(), _variant_t());
	ThrowOnError();

	m_hr = m_pService->GetFolder(_T("\\"), &m_pFolder);
	ThrowOnError();
}

Installer::~Installer()
{
	::CoUninitialize();
}

BOOL Installer::IsTaskExist()
{
	CComPtr<IRegisteredTask> pTask;
	m_pFolder->GetTask(_bstr_t(TASK_NAME), &pTask);
	ThrowOnError();
	return (pTask != NULL);
}

Installer::InstallStatus Installer::ExecuteInstall()
{
	DWORD dwLen = MAX_PATH;
	CAtlString sSelfPath;
	DWORD dwRes;
	do
	{
		dwRes = ::GetModuleFileName( NULL, sSelfPath.GetBuffer(dwLen), dwLen);
	} 
	while ((dwRes == dwLen) && (::GetLastError() == ERROR_INSUFFICIENT_BUFFER));

	InstallStatus result = isUnknown;
	try
	{
		result = CreateTask( sSelfPath );
	}
	catch(...)
	{
		result = isUnknown;
	}

	CAtlString message;
	CAtlString title;
	title.LoadString(IDS_TITLE_INSTALATION);
	if (result == isInstalled)
	{
		CAtlString platform;
#ifdef _WIN64
		platform.LoadString(IDS_X64);
#else
		platform.LoadString(IDS_X86);
#endif
		message.Format(IDS_INSTALLED, platform);
		::MessageBox( NULL, message, title, MB_ICONINFORMATION );
	}
	else if (result == isPrivileges)
	{
		message.LoadString(IDS_INSTALATION_INFO);
		::MessageBox( NULL, message, title, MB_ICONINFORMATION );
	}
	else if (result == isAlready)
	{
		message.LoadString(IDS_ALREADY);
		::MessageBox( NULL, message, title, MB_ICONINFORMATION );
	}
	return result;
}

Installer::InstallStatus Installer::ExecuteUninstall()
{
	InstallStatus result = isUnknown;

	m_hr = m_pFolder->DeleteTask(_bstr_t(TASK_NAME), 0);
	if (SUCCEEDED(m_hr))
	{
		result = isUninstalled; 
	}
	if (m_hr == E_ACCESSDENIED)
		result = isPrivileges;

	CAtlString message;
	CAtlString title;
	title.LoadString(IDS_TITLE_INSTALATION);
	if (result == isUninstalled)
	{
		CAtlString platform;
#ifdef _WIN64
		platform.LoadString(IDS_X64);
#else
		platform.LoadString(IDS_X86);
#endif
		message.Format(IDS_UNINSTALLED, platform);
		::MessageBox( NULL, message, title, MB_ICONINFORMATION );
	}
	else if (result == isPrivileges)
	{
		message.LoadString(IDS_UNINSTALL_USAGE_INFO);
		::MessageBox( NULL, message, title, MB_ICONINFORMATION );
	}
	else if (result == isAlready)
	{
		message.LoadString(IDS_ALREADY);
		::MessageBox( NULL, message, title, MB_ICONINFORMATION );
	}

	return result;
}

Installer::InstallStatus Installer::CreateTask(CAtlString &sSelfPath)
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
	if (SUCCEEDED(m_hr))
		return isInstalled; 
	if (m_hr == E_ACCESSDENIED)
		return isPrivileges;

	return isUnknown;
}
