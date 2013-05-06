#include "ComInitializer.h"

class Installer
{
public:
	static HRESULT Execute(LPCTSTR taskname, LPCTSTR taskparam, BOOL &bInstall, ComInitializer &comObject);
	static HRESULT Install(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &comObject);
	static HRESULT Uninstall(LPCTSTR taskname, ComInitializer &comObject);
private:
	ComInitializer &m_comObj;
	HRESULT m_hr;
	CComPtr<ITaskService> m_pService;
	CComPtr<ITaskFolder> m_pFolder;
	LPCTSTR m_lpTaskName;
	LPCTSTR m_lpTaskParam;

	Installer(LPCTSTR taskname, LPCTSTR taskparam, ComInitializer &comObject);
	HRESULT ThrowOnError(LPCSTR message = NULL);

	BOOL IsTaskExist();

	HRESULT ExecuteInstall();
	HRESULT ExecuteUninstall();

	HRESULT CreateTask(CAtlString &sSelfPath);	
};