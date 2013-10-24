#include "stdafx.h"
#include "CmdLineParser.h"


CCmdLineParser::CCmdLineParser(LPCTSTR szCmdLine):
	m_strCmdLine(szCmdLine != NULL? szCmdLine: _T("")),
	m_dwFlags(0)
{
	SeparateParams();
}

CCmdLineParser::~CCmdLineParser(void)
{
}


VOID CCmdLineParser::SeparateParams()
{
	int curPos = 0;

	CAtlString resToken = m_strCmdLine.Tokenize(_T("/"), curPos);
	while (curPos > 0)
	{
		resToken.Trim();
		m_arrayParams.Add(resToken);
		resToken = m_strCmdLine.Tokenize(_T("/"), curPos);
	};

	/*resToken = m_strCmdLine.Right(curPos);
	m_arrayParams.Add(resToken);*/
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
	for (auto i = 0; i < m_arrayParams.GetCount(); i++)
	{
		if (m_arrayParams[i].CompareNoCase(param) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}