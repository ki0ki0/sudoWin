#include "stdafx.h"
#include "CmdLineParser.h"


CCmdLineParser::CCmdLineParser(LPCTSTR szCmdLine):
	m_strCmdLine(szCmdLine != NULL? szCmdLine: _T("")),
	m_dwFlags(0)
{
}

CCmdLineParser::~CCmdLineParser(void)
{
}

DWORD CCmdLineParser::AddFlag(LPCTSTR szName, DWORD dwValue)
{
	if (CheckCmdParam( szName))
	{
		m_dwFlags |= dwValue;
	}
	return m_dwFlags;
}

BOOL CCmdLineParser::CheckCmdParam( LPCTSTR param)
{
	int pos = m_strCmdLine.Find(param);
	if (pos == -1)
		return FALSE;
	
	size_t len = _tcslen(param);
	if (pos+len == m_strCmdLine.GetLength())
		return TRUE;
	
	if (m_strCmdLine[pos+len] == ' ')
		return TRUE;

	return FALSE;
}