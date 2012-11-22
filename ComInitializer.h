#pragma once
class ComInitializer
{
public:
	ComInitializer(void);
	~ComInitializer(void);

	void ThrowOnError(HRESULT hr);
};

