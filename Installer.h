#include "ComInitializer.h"

#ifdef _WIN64
#define TASK_NAME	_T("sudoWin64")
#else
#define TASK_NAME	_T("sudoWin")
#endif

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

	static InstallStatus Execute(ComInitializer &comObject);
	static InstallStatus Install(ComInitializer &comObject);
	static InstallStatus Uninstall(ComInitializer &comObject);
private:
	ComInitializer &m_comObj;
	HRESULT m_hr;
	CComPtr<ITaskService> m_pService;
	CComPtr<ITaskFolder> m_pFolder;

	Installer(ComInitializer &comObject);
	void ThrowOnError();

	BOOL IsTaskExist();

	void ExecuteInstall();
	void ExecuteUninstall();

	void CreateTask(CAtlString &sSelfPath);	

	static InstallStatus ProcessError( int code, BOOL isInstalled);
};