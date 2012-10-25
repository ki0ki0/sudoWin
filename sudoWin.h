#pragma once

#include "resource.h"

#ifdef _WIN64
#define TASK_NAME	_T("sudoWin64")
#else
#define TASK_NAME	_T("sudoWin")
#endif

static LPCTSTR c_szRegPath = _T("Software\\sudoWin");
static LPCTSTR c_szExecApp = _T("App");
static LPCTSTR c_szExecArgs = _T("Args");
static LPCTSTR c_szExecDir = _T("Dir");
static LPCTSTR c_szExecIn = _T("In");
static LPCTSTR c_szExecOut = _T("Out");
static LPCTSTR c_szExecErr = _T("Err");
static LPCTSTR c_szExecId = _T("Id");

static LPCTSTR c_szEventRun = _T("sudoWin%dRUN");
static LPCTSTR c_szEventExit = _T("sudoWin%dEXIT");

static DWORD c_dwRunTimeout = 1000;

enum enErrorCodes
{
	ecNoError = 0,
	ecRegSet,
	ecRunTask,
	ecRegGet,
	ecRegSHExec,
	ecInstall,
	ecInstallFailed,
    ecUnknown = -1
};
