#include "stdafx.h"

#include "regvalue.h"

void RegValue::CheckError()
{
	if (m_stResult == ERROR_ACCESS_DENIED)
		throw "Access to register key is denied";
	else if (m_stResult == ERROR_PATH_NOT_FOUND) // todo: check is this correct code
		throw "Register key is not found";
	else if (m_stResult != ERROR_SUCCESS)
		throw "Unknown error";
}

RegValue::RegValue(HKEY hkey, LPCTSTR lpSubKey/*, BOOL bAllowWrite*/) // todo: is this required?
{
	HKEY hKey;
	m_stResult = ::RegOpenKey(hkey, lpSubKey, &hKey);
	CheckError();

	m_hKey.Attach(hKey);
}

CAtlString RegValue::GetString(LPCTSTR lpValue)
{
	LONG lLen;
	m_stResult = ::RegQueryValue( (HKEY)m_hKey.m_h, lpValue, NULL, &lLen);
    CheckError();

    CAtlString str;
	str.GetBuffer( lLen / sizeof(TCHAR) + 1 );
	m_stResult = ::RegQueryValue( HKEY_CURRENT_USER, lpValue, str.GetBuffer(), &lLen);
    CheckError();

    str.ReleaseBuffer(lLen);
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
	m_stResult = ::RegSetValue( (HKEY)m_hKey.m_h, lpValue, REG_SZ, lpText, dwLen);
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