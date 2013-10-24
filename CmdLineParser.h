#pragma once
class CCmdLineParser
{
public:
	CCmdLineParser(LPCTSTR szCmdLine);
	~CCmdLineParser(void);

	DWORD AddFlag(LPCTSTR szName, DWORD dwValue);
private:
	CAtlString m_strCmdLine;
	CAtlArray<CAtlString> m_arrayParams;
	DWORD m_dwFlags;

	BOOL CheckCmdParam( LPCTSTR param);
	VOID SeparateParams();
};

