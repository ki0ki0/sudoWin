#pragma once

#include "resource.h"

enum enErrorCodes
{
	ecNoError = 0,
	ecSimultanius,
	ecRegSet,
	ecRunTask,
	ecRegGet,
	ecRegSHExec,
	ecInstall,
	ecInstallFailed,
    ecUnknown = -1
};
