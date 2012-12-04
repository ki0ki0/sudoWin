static LPCTSTR c_szRegPath = _T("Software\\sudoWin");
static LPCTSTR c_szExecApp = _T("App");
static LPCTSTR c_szExecArgs = _T("Args");
static LPCTSTR c_szExecDir = _T("Dir");
static LPCTSTR c_szExecIn = _T("In");
static LPCTSTR c_szExecOut = _T("Out");
static LPCTSTR c_szExecErr = _T("Err");
static LPCTSTR c_szExecId = _T("Id");

class Params
{
private:
	Params(){}
public:
	static void Save( LPCTSTR lpCmdLine);
	static void Load( CAtlString &sDir, CAtlString &sApp, CAtlString &sArgs, DWORD &dwProcessId);
	static void Clear();
};