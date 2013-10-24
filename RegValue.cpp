#include "stdafx.h"

#include "regvalue.h"

#include "win32_exception.h"

void RegValue::CheckError(LPCSTR szMessage)
{
	if (m_stResult != ERROR_SUCCESS)
	{
		LPCSTR szMessageTmp = szMessage != NULL? szMessage: "Error while operating with register";
		throw win32_exception(szMessageTmp, m_stResult);
	}
}

RegValue::RegValue(HKEY hkey, LPCTSTR lpSubKey, BOOL bIsWritable):
	m_bIsWritable(bIsWritable)
{
	HKEY hKey;
	m_stResult = ::RegCreateKeyEx(hkey, lpSubKey, 0, NULL, 0, 
		m_bIsWritable? KEY_WRITE: KEY_READ, NULL, &hKey, NULL);
	CheckError("Can't open registry key");	
	m_hKey.Attach(hKey);
}

void RegValue::CheckReadPerissions()
{
	if (m_bIsWritable)
		throw std::logic_error("This key is for write");
}

void RegValue::CheckWritePerissions()
{
	if (m_bIsWritable == false)
		throw std::logic_error("This key is for read");
}

CAtlString RegValue::GetString(LPCTSTR lpValue)
{
	CheckReadPerissions();

	DWORD dLen;
	DWORD dwType;
	m_stResult = ::RegQueryValueEx( (HKEY)m_hKey.m_h, lpValue, 0, &dwType, NULL, &dLen);
    CheckError("Can't query registry value");

	if (dwType != REG_SZ)
		throw std::runtime_error("Incompatible types");

    CAtlString str;
	m_stResult = ::RegQueryValueEx( (HKEY)m_hKey.m_h, lpValue, 0, NULL,
		(LPBYTE)str.GetBuffer(dLen / sizeof(TCHAR) + 1), &dLen);
    CheckError("Can't query registry value");

    str.ReleaseBuffer(dLen / sizeof(TCHAR));
	str.ReleaseBuffer();
	return str;
}

DWORD RegValue::GetDword(LPCTSTR lpValue)
{
	CheckReadPerissions();

	DWORD dwType;
	DWORD dwVal;
	DWORD dwLen;
	m_stResult = ::RegQueryValueEx( (HKEY)m_hKey.m_h, lpValue, 0, &dwType, NULL, &dwLen);
	CheckError("Can't query registry value");

	if ((dwType != REG_DWORD) || (dwLen != sizeof(dwVal)))
		throw std::runtime_error("Incompatible types");
	m_stResult = ::RegQueryValueEx( (HKEY)m_hKey.m_h, lpValue, 0, &dwType, (LPBYTE)&dwVal, &dwLen);
	CheckError("Can't query registry value");
	return dwVal;
}

void RegValue::SetString(LPCTSTR lpValue, LPCTSTR lpText, DWORD dwLen)
{
	CheckWritePerissions();

	m_stResult = ::RegSetValueEx( (HKEY)m_hKey.m_h, lpValue, 0, REG_SZ, (LPBYTE)lpText, dwLen * sizeof(lpText[0]));
    CheckError("Can't set registry value");
}

void RegValue::SetDword(LPCTSTR lpValue, DWORD dwVal)
{
	CheckWritePerissions();

	m_stResult = ::RegSetValueEx( (HKEY)m_hKey.m_h, lpValue, 0, REG_DWORD, (LPBYTE)&dwVal, sizeof(dwVal));
    CheckError("Can't set registry value");
}

void RegValue::Delete(LPCTSTR lpValue)
{
	CheckWritePerissions();

	m_stResult = ::RegDeleteValue( (HKEY)m_hKey.m_h, lpValue);
    CheckError("Can't delete registry value");
}