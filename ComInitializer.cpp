#include "stdafx.h"
#include "ComInitializer.h"
#include "win32_exception.h"


ComInitializer::ComInitializer(void)
{
	HRESULT hr = ::CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	ThrowOnError(hr);
}


ComInitializer::~ComInitializer(void)
{
	::CoUninitialize();
}

void ComInitializer::ThrowOnError(HRESULT hr, LPCSTR message)
{
	if (FAILED(hr))
	{
		throw win32_exception(message, hr);
	}
}