class Installer
{
public:
	enum InstallStatus
	{
		isUnknown,
		isCanceled,
		isPrivileges,
		isInstalled,
		isUninstalled,
		isAlready
	};

	static InstallStatus Execute();
	static InstallStatus Install();
	static InstallStatus Uninstall();
private:
	HRESULT m_hr;
	CComPtr<ITaskService> m_pService;
	CComPtr<ITaskFolder> m_pFolder;

	Installer();
	~Installer();
	void ThrowOnError();

	BOOL IsTaskExist();

	InstallStatus ExecuteInstall();
	InstallStatus ExecuteUninstall();

	InstallStatus CreateTask(CAtlString &sSelfPath);	
};