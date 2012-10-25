#pragma once

#include "resource.h"

#ifdef _WIN64
#define TASK_NAME	_T("sudoWin64")
#else
#define TASK_NAME	_T("sudoWin")
#endif

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
