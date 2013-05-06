#include "stdafx.h"

#include "installer.h"
#include "resource.h"

HRESULT Installer::Execute(LPCTSTR taskname, LPCTSTR taskparam, BOOL &bInstall, ComInitializer &comObject)
{
	HRESULT hrStatus = S_OK;

	Installer inst(taskname, taskparam, comObject);
	BOOL bIsInstalled = inst.IsTaskExist();
	if (bIsInstalled)
	{
		bInstall = FALSE;
		hrStatus = inst.ExecuteUninstall();
	}
	else
	{
		bInstall = TRUE;
		hrStatus = inst.ExecuteInstall();
	}
	return hrStatus;
}

HRESULT Installer::Install(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &comObject)
{
	HRESULT hrStatus = S_OK;

	Installer inst(taskname, taskparam, comObject);
	BOOL bIsInstalled = inst.IsTaskExist();
	if (bIsInstalled)
	{
		hrStatus = S_FALSE;
	}
	else
	{
		hrStatus = inst.ExecuteInstall();
	}

	return hrStatus;
}

HRESULT Installer::Uninstall(LPCTSTR taskname, ComInitializer &comObject)
{
	HRESULT hrStatus = S_OK;

	Installer inst(taskname, nullptr, comObject);
	BOOL bIsInstalled = inst.IsTaskExist();
	if (bIsInstalled == FALSE)
	{
		hrStatus = S_FALSE;
	}
	else
	{
		hrStatus = inst.ExecuteUninstall();
	}
	return hrStatus;
}

HRESULT Installer::ThrowOnError(LPCSTR message)
{
	m_comObj.ThrowOnError(m_hr);
	return m_hr;
}

Installer::Installer(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &com):
	m_comObj(com), m_lpTaskName(taskname), m_lpTaskParam(taskparam)
{
	m_hr = CoCreateInstance( CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&m_pService );  
	ThrowOnError("Can't create task scheduler instance");

	m_hr = m_pService->Connect( _variant_t(), _variant_t(), _variant_t(), _variant_t());
	ThrowOnError("Can't connect to local task scheduler");

	m_hr = m_pService->GetFolder(_T("\\"), &m_pFolder);
	ThrowOnError("Can't get Task scheduler root folder");
}

BOOL Installer::IsTaskExist()
{
	CComPtr<IRegisteredTask> pTask;
	m_hr = m_pFolder->GetTask(_bstr_t(m_lpTaskName), &pTask);;
	return SUCCEEDED(m_hr);
}

HRESULT Installer::ExecuteInstall()
{
	DWORD dwLen = MAX_PATH;
	CAtlString sSelfPath;
	DWORD dwRes;
	do
	{
		dwRes = ::GetModuleFileName( NULL, sSelfPath.GetBuffer(dwLen), dwLen);
	} 
	while ((dwRes == dwLen) && (::GetLastError() == ERROR_INSUFFICIENT_BUFFER));

	return CreateTask( sSelfPath );
}

HRESULT Installer::ExecuteUninstall()
{
	m_hr = m_pFolder->DeleteTask(_bstr_t(m_lpTaskName), 0);
	return ThrowOnError("Can't delete task");
}

HRESULT Installer::CreateTask(CAtlString &sSelfPath)
{
	CComPtr<ITaskDefinition> pDefinition;
	m_hr = m_pService->NewTask(0, &pDefinition);
	ThrowOnError("Can't create new task");

	//TaskSettings
	CComPtr<ITaskSettings> pTaskSettings;
	m_hr = pDefinition->get_Settings(&pTaskSettings);
	ThrowOnError("Can't get task settings");

	m_hr = pTaskSettings->put_DisallowStartIfOnBatteries(FALSE);
	ThrowOnError("Can't set DisallowStartIfOnBatteries");

	m_hr = pTaskSettings->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
	ThrowOnError("Can't set MultipleInstances to parallel");

	//Principal
	CComPtr<IPrincipal> pPrincipal;
	m_hr = pDefinition->get_Principal(&pPrincipal);
	ThrowOnError("Can't get Principal");

	m_hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	ThrowOnError("Can't set highest RunLevel");

	//IActionCollection
	CComPtr<IActionCollection> pActionCollection;
	m_hr = pDefinition->get_Actions( &pActionCollection );
	ThrowOnError("Can't get Actions");

	CComPtr<IAction> pAction;
	m_hr = pActionCollection->Create( TASK_ACTION_EXEC, &pAction );
	ThrowOnError("Can't createnew Action");

	CComPtr<IExecAction> pExecAction;
	m_hr = pAction->QueryInterface( IID_IExecAction, (void**)&pExecAction );
	ThrowOnError("Can't query Exec Action");

	m_hr = pExecAction->put_Path( _bstr_t(sSelfPath.GetString()) );
	ThrowOnError("Can't put execute path");

	m_hr = pExecAction->put_Arguments( _bstr_t(m_lpTaskParam) );
	ThrowOnError("Can't put execute arguments");

	// Register
	CComPtr<IRegisteredTask> pRegisteredTask;
	m_hr = m_pFolder->RegisterTaskDefinition( _bstr_t(m_lpTaskName), pDefinition, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pRegisteredTask);
	return ThrowOnError("Can't register task definition");
}
