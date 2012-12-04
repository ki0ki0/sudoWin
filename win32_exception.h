#pragma once

class win32_exception
	: public std::exception
	{	// base of all logic-error exceptions
public:
	typedef std::exception _Mybase;

	DWORD m_dwErrorCode;

	explicit win32_exception(const std::string& _Message, DWORD dwErrorCode)
		: _Mybase(_Message.c_str()), m_dwErrorCode(dwErrorCode)
		{	// construct from message string
		}

	explicit win32_exception(const char *_Message, DWORD dwErrorCode)
		: _Mybase(_Message), m_dwErrorCode(dwErrorCode)
		{	// construct from message string
		}

	};