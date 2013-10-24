#pragma once

#define WIN32_DEFAULT_MESSAGE "Win32 ErrorCode"

class win32_exception
	: public std::exception
{	// base of all logic-error exceptions

	typedef std::exception _Mybase;

	HRESULT m_hrStatus;

public:
	explicit win32_exception( HRESULT hrStatus)
		: _Mybase(WIN32_DEFAULT_MESSAGE), m_hrStatus(hrStatus)
	{	// construct from message string
	}

	explicit win32_exception(const std::string& _Message, HRESULT hrStatus)
		: _Mybase(_Message.c_str()), m_hrStatus(hrStatus)
	{	// construct from message string
	}

	explicit win32_exception(const char *_Message, HRESULT hrStatus)
		: _Mybase(_Message? _Message: WIN32_DEFAULT_MESSAGE), m_hrStatus(hrStatus)
	{	// construct from message string
	}

	explicit win32_exception(const char *_Message, DWORD dwCode)
		: _Mybase(_Message? _Message: WIN32_DEFAULT_MESSAGE), m_hrStatus(HRESULT_FROM_WIN32(dwCode))
	{	// construct from message string
	}

	HRESULT GetStatus()
	{
		return m_hrStatus;
	}
};