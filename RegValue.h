class RegValue
{
public:
	RegValue(HKEY hkey, LPCTSTR lpSubKey);
	CAtlString GetString(LPCTSTR lpValue);
	DWORD GetDword(LPCTSTR lpValue);
	void SetString(LPCTSTR lpValue, LPCTSTR lpText, DWORD dwLen);
	void SetDword(LPCTSTR lpValue, DWORD dwVal);
	void Delete(LPCTSTR lpValue);
private:
	CHandle	m_hKey;
	LSTATUS m_stResult;
	void CheckError();
};