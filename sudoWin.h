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
/*
static LPCTSTR c_szMBTitle = _T("sudoWin installation");
static LPCTSTR c_szMBInstallInfo = _T("\
To install sudoWin, You should place the file you just started \
where ever you want to use it from(You can change file name, for example \"sudo\" or \"su\"), \
and run with administrator privileges.\n\n\
It clould be good idea to place it in folder that is in PATH environment variable \
(it could be \"System32\" folder, or \"SysWOW64\" for x86 version under x64 OS), \
so you may run it without path specifying!\n\n\
Usage: sudoWin <app path> [<parameter1> [<parameter2> ...]]\n\
Usage example:\n\
sudoWin regedit\n\
sudoWin \"C:\\Program Files\\Far2\\far.exe\"\n");

static LPCTSTR c_szMBUsageInfo = _T("\
Usage: sudoWin <app path> [<parameter1> [<parameter2> ...]]\n\
Usage example:\n\
sudoWin regedit\n\
sudoWin \"C:\\Program Files\\Far2\\far.exe\"\n\n\
To uninstall sudoWin run with administrator privileges.");

static LPCTSTR c_szMBAlready = _T("Nothiing to do.");

#ifdef _WIN64
static LPCTSTR c_szMBInstallSuccess = _T("\
You have succsessfuly installed sudoWin x64 verison.\n");
static LPCTSTR c_szMBUninstallSuccess = _T("\
You have succsessfuly uninstalled sudoWin x64 verison.\n");
#else
static LPCTSTR c_szMBInstallSuccess = _T("\
You have succsessfuly installed sudoWin x86 version.\n");
static LPCTSTR c_szMBUninstallSuccess = _T("\
You have succsessfuly uninstalled sudoWin x86 verison.\n");
#endif*/