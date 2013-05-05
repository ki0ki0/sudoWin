#include "stdafx.h"
#include "CppUnitTest.h"
#include "../RegValue.h"
#include "../Params.h"

#include <direct.h>

#include "../win32_exception.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace sudoWinTest
{		
	TEST_CLASS(TestRegValue)
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

		TEST_METHOD(TestRegValueStringToDword)
		{
			RegValue rw(HKEY_CURRENT_USER, L"sudowin\test", true);
			CAtlString str("asdasdas");
			rw.SetString(L"dword", str, str.GetLength() + 1);
			RegValue rr(HKEY_CURRENT_USER, L"sudowin\test", false);

			auto f1 = [&rr] {
				rr.GetDword(L"dword");
			};
			Assert::ExpectException<std::runtime_error>(f1, L"DWORD type check isn't valid");
			rw.Delete(L"dword");
		}

		TEST_METHOD(TestRegValueDwordToString)
		{
			RegValue rw(HKEY_CURRENT_USER, L"sudowin\test", true);
			rw.SetDword(L"dword", 12345);
			RegValue rr(HKEY_CURRENT_USER, L"sudowin\test", false);

			auto f1 = [&rr] {
				rr.GetString(L"dword");
			};
			Assert::ExpectException<std::runtime_error>(f1, L"String type check isn't valid");
			rw.Delete(L"dword");
		}
	};
}