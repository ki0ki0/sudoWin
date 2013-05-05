#include "stdafx.h"
#include "CppUnitTest.h"
#include "../RegValue.h"
#include "../Params.h"

#include <direct.h>

#include "../win32_exception.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace sudoWinTest
{		
	TEST_CLASS(TestParams)
	{
	public:

		TEST_METHOD(TestParamsSaveInput)
		{
			auto f1 = [] {Params::Save(nullptr);};
			Assert::ExpectException<std::invalid_argument>(f1, L"No input parameters check");
		}

		TEST_METHOD(TestParamsSaveLoadClear)
		{
			CAtlString wdr(L"c:\\users");
			CAtlString path(L"d:\\1.exe");
			CAtlString args(L"123 qwe rty");
			CAtlString tmp(path);
			tmp.Append(L" ");
			tmp.Append(args);
			_wchdir(wdr);
			Params::Save(tmp.GetString());
			CAtlString dir, exe, par;
			DWORD id;
			Params::Load( dir, exe, par, id);
			Assert::IsTrue(path == exe, L"Exe path parsing error");
			Assert::IsTrue(par == args, L"Arguments parsing error");
			Assert::IsTrue(id == ::GetCurrentProcessId(), L"ProcessId is incorrect");
			Assert::IsTrue(wdr == dir, L"Working dir isn't correct");
		}
		
		TEST_METHOD(TestParamsSaveClear)
		{
			CAtlString wdr(L"c:\\users");
			CAtlString path(L"d:\\1.exe");
			CAtlString args(L"123 qwe rty");
			CAtlString tmp(path);
			tmp.Append(L" ");
			tmp.Append(args);
			_wchdir(wdr);
			Params::Save(tmp.GetString());

			CAtlString dir, exe, par;
			DWORD id;
			Params::Clear();
			try
			{
				Params::Load( dir, exe, par, id);
			}
			catch (win32_exception ex)
			{
				Assert::IsTrue( ex.m_dwErrorCode == ERROR_FILE_NOT_FOUND,
					L"Exe path parsing error");
			}
		}
	};
}