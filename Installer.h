#include "ComInitializer.h"

class Installer
{
public:
	enum InstallStatus
	{
		isUnknown = 0,
		isCanceled,
		isPrivileges,
		isInstalled,
		isUninstalled,
		isAlready
	};

	static InstallStatus Execute(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &comObject, BOOL bSilent = FALSE);
	static InstallStatus Install(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &comObject, BOOL bSilent = FALSE);
	static InstallStatus Uninstall(LPCTSTR taskname, ComInitializer &comObject, BOOL bSilent = FALSE);
private:
	ComInitializer &m_comObj;
	HRESULT m_hr;
	CComPtr<ITaskService> m_pService;
	CComPtr<ITaskFolder> m_pFolder;
	LPCTSTR m_lpTaskName;
	LPCTSTR m_lpTaskParam;

	Installer(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &comObject);
	void ThrowOnError();

	BOOL IsTaskExist();

	void ExecuteInstall();
	void ExecuteUninstall();

	void CreateTask(CAtlString &sSelfPath);	

	static InstallStatus ProcessError( int code, BOOL isInstalled, BOOL bSilent);
};