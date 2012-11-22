#include "stdafx.h"
#include "ComInitializer.h"


ComInitializer::ComInitializer(void)
{
	HRESULT hr = ::CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	ThrowOnError(hr);
}


ComInitializer::~ComInitializer(void)
{
	::CoUninitialize();
}

void ComInitializer::ThrowOnError(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw HRESULT_CODE(hr);
	}
}