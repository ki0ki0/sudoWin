#include "stdafx.h"

#include "regvalue.h"

void RegValue::CheckError()
{
	if (m_stResult != ERROR_SUCCESS)
	{
		throw m_stResult;
	}
}

RegValue::RegValue(HKEY hkey, LPCTSTR lpSubKey, BOOL bAllowWrite)
{
	HKEY hKey;
	m_stResult = ::RegCreateKeyEx(hkey, lpSubKey, 0, NULL, 0, 
		bAllowWrite? KEY_WRITE: KEY_READ, NULL, &hKey, NULL);
	CheckError();

	m_hKey.Attach(hKey);
}

CAtlString RegValue::GetString(LPCTSTR lpValue)
{
	DWORD dLen;
	m_stResult = ::RegQueryValueEx( (HKEY)m_hKey.m_h, lpValue, 0, NULL, NULL, &dLen);
    CheckError();

    CAtlString str;
	m_stResult = ::RegQueryValueEx( (HKEY)m_hKey.m_h, lpValue, 0, NULL,
		(LPBYTE)str.GetBuffer(dLen / sizeof(TCHAR) + 1), &dLen);
    CheckError();

    str.ReleaseBuffer(dLen / sizeof(TCHAR));
	str.ReleaseBuffer();
	return str;
}

DWORD RegValue::GetDword(LPCTSTR lpValue)
{
	DWORD dwType;
	DWORD dwVal;
	DWORD dwLen;
	m_stResult = ::RegQueryValueEx( (HKEY)m_hKey.m_h, lpValue, 0, &dwType, NULL, &dwLen);
	CheckError();

	if ((dwType != RRF_RT_REG_BINARY) || (dwLen != sizeof(dwVal)))
		throw "Wrong type of value";
	m_stResult = ::RegQueryValueEx( (HKEY)m_hKey.m_h, lpValue, 0, &dwType, (LPBYTE)&dwVal, &dwLen);
	CheckError();
	return dwVal;
}

void RegValue::SetString(LPCTSTR lpValue, LPCTSTR lpText, DWORD dwLen)
{
	m_stResult = ::RegSetValueEx( (HKEY)m_hKey.m_h, lpValue, 0, REG_SZ, (LPBYTE)lpText, dwLen);
    CheckError();
}

void RegValue::SetDword(LPCTSTR lpValue, DWORD dwVal)
{
	m_stResult = ::RegSetValueEx( (HKEY)m_hKey.m_h, lpValue, 0, RRF_RT_REG_BINARY, (LPBYTE)&dwVal, sizeof(dwVal));
    CheckError();
}

void RegValue::Delete(LPCTSTR lpValue)
{
	m_stResult = ::RegDeleteValue( (HKEY)m_hKey.m_h, lpValue);
    CheckError();
}