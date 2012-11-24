#pragma once
class Runner
{
public:
	static int NewCmd(LPCTSTR taskname, LPTSTR lpCmdLine);
	static int ExecuteCmd();

private:
	static const LPCTSTR c_szEventRun;
	static const LPCTSTR c_szEventExit;

	static const DWORD c_dwRunTimeout;
};

