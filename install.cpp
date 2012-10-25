#include "stdafx.h"

#include "installer.h"
#include "sudowin.h"

Installer::InstallStatus Installer::Execute()
{
	InstallStatus result;

	Installer *inst = new Installer();
	BOOL isInstalled = inst->IsTaskExist();
	if (isInstalled)
	{
		result = inst->ExecuteUninstall();
	}
	else
	{
		result = inst->ExecuteInstall();
	}

	if (result == isPrivileges)
	{
		if (isInstalled)
			::MessageBox( NULL, c_szMBUsageInfo, c_szMBTitle, MB_ICONINFORMATION );
		else
			::MessageBox( NULL, c_szMBInstallInfo, c_szMBTitle, MB_ICONINFORMATION );
	}
	else if (result == isAlready)
		::MessageBox( NULL, c_szMBAlready, c_szMBTitle, MB_ICONINFORMATION );
	

	return result;
}

Installer::InstallStatus Installer::Install()
{
	InstallStatus result;

	Installer *inst = new Installer();
	if (inst->IsTaskExist())
	{
		result = isAlready;
	}
	else
	{
		result = inst->ExecuteInstall();
	}

	if (result == isPrivileges)
		::MessageBox( NULL, c_szMBInstallInfo, c_szMBTitle, MB_ICONINFORMATION );

	return result;
}

Installer::InstallStatus Installer::Uninstall()
{
	InstallStatus result;

	Installer *inst = new Installer();
	if (inst->IsTaskExist() == FALSE)
	{
		result = isAlready;
	}
	else
	{
		result = inst->ExecuteUninstall();
	}

	if (result == isPrivileges)
		::MessageBox( NULL, c_szMBUsageInfo, c_szMBTitle, MB_ICONINFORMATION );

	return result;
}


void Installer::ThrowOnError()
{
	if (FAILED(m_hr))
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

	try
	{
		return CreateTask( sSelfPath );
	}
	catch(...)
	{
		return isUnknown;
	}
}

Installer::InstallStatus Installer::ExecuteUninstall()
{
	m_hr = m_pFolder->DeleteTask(_bstr_t(TASK_NAME), 0);
	if (SUCCEEDED(m_hr))
	{
		::MessageBox( NULL, c_szMBUninstallSuccess, c_szMBTitle, MB_ICONINFORMATION );
		return isUninstalled; 
	}
	if (m_hr == E_ACCESSDENIED)
		return isPrivileges;
	return isUnknown;
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
	{
		::MessageBox( NULL, c_szMBInstallSuccess, c_szMBTitle, MB_ICONINFORMATION );
		return isInstalled; 
	}
	if (m_hr == E_ACCESSDENIED)
		return isPrivileges;

	return isUnknown;
}
