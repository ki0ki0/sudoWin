#include "stdafx.h"
#include "CppUnitTest.h"
#include "../RegValue.h"
#include "../Params.h"

#include <direct.h>

#include "../win32_exception.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace sudoWinTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestRegValuePermissions)
		{
			RegValue rr(HKEY_CURRENT_USER, L"sudowin\test", false);
			auto f1 = [&rr] { 
				CAtlString str("asdasdas");
				rr.SetString(L"str", str, str.GetLength() + 1); 
			};
			Assert::ExpectException<std::logic_error>(f1, L"Writing for read only key is allowed");

			RegValue rw(HKEY_CURRENT_USER, L"sudowin\test", true);
			auto f2 = [&rw] { 
				rw.GetString(L"str"); 
			};
			Assert::ExpectException<std::logic_error>(f2, L"Reading for write only key is allowed");

			auto f3 = [&rr] { 
				CAtlString str("asdasdas");
				rr.Delete(L"str"); 
			};
			Assert::ExpectException<std::logic_error>(f3, L"Deleting for read only key is allowed");
		}

		TEST_METHOD(TestRegValueStringWriteRead)
		{
			RegValue rw(HKEY_CURRENT_USER, L"sudowin\test", true);
			CAtlString str("asdasdas");
			rw.SetString(L"str", str, str.GetLength() + 1);
			RegValue rr(HKEY_CURRENT_USER, L"sudowin\test", false);
			Assert::IsTrue(str == rr.GetString(L"str"), L"String is not saved in correct way");
			rw.Delete(L"str");
		}

		TEST_METHOD(TestRegValueDwordWriteRead)
		{
			RegValue rw(HKEY_CURRENT_USER, L"sudowin\test", true);
			DWORD val = 12345;
			rw.SetDword(L"dword", val);
			RegValue rr(HKEY_CURRENT_USER, L"sudowin\test", false);
			Assert::IsTrue( val == rr.GetDword(L"dword"), L"DWORD is not saved in correct way");
			rw.Delete(L"dword");
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