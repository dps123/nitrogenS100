//////////////////////////////////////////////////////////////////////
//
// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File Name: StudioFile.cpp
// C++ Compiler Used: Microsoft eVC++ 3.0
// Produced By: SofTech Systems Inc., New Baden, Il 62265
// File Creation Date: 07/14/2002
// Date Last Modified: 07/14/2002
// Copyright (c) 2002 SofTech System Inc.
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
 
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  
USA

The CStudioFile class is intended for WinCE MFC applications because MFC for
WinCE did not implement the CStdioFile text file functions.  You should
use the CStdioFile in Win32 programs.  

This class implements both UNICODE and ASCII text file reads and writes. If the
format is UNICODE, it is assumed to contain 16-bit strings.  Other UNICODE formats 
cannot be read by WinCE programs. It is the caller's responsibility to know which format 
the file is in.

Because of file buffering, programmers should not mix the CFile::Read and
CFile::Write with the ReadString methods in this class.  Doing so will cause
unpredictable results.

CFile::typeText is stripped from the open flags when the Open method is called
to avoid the ASSERT in the base class.  It is not necessary to include this 
flag when using the CStudioFile class.
*/
// ----------------------------------------------------------- // 

#include "stdafx.h"
#include "StudioFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStudioFile::CStudioFile()
{
	m_nChars = 0;
	m_nextout = 0;
	memset(m_ibuf, 0, sizeof(m_ibuf));
}
CStudioFile::~CStudioFile() {
	CloseHandle(f);
}
BOOL CStudioFile::Open(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
	)
{
	f = CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	_eof=(f == INVALID_HANDLE_VALUE);
	return f != INVALID_HANDLE_VALUE;
}
BOOL CStudioFile::isEOF()
{
	return _eof;
}

wchar_t* CStudioFile::ReadString(wchar_t* lpsz, UINT nMax)
// Reads text data into a buffer, up to a limit of nMax -1 characters, 
// from the file associated with the CStudioFile object. 
// Reading is stopped by the first newline character. If, 
// in that case, fewer than nMax-1 characters have been read,
// a newline character ('\n') is stored in the buffer. A null character
// (‘\0’) is appended in either case. 
{
	wchar_t c;
	UINT i;
	for(i = 0; i < nMax-1; i++)
	{
		// If the buffer is empty or all data has been
		// returned, get more data from the file.
		if(m_nextout == m_nChars)
		{
			ReadFile(f, m_ibuf, _MAX_BUFSIZE*sizeof(wchar_t), &m_nChars, false);
			m_nChars /= sizeof(wchar_t);
			m_nextout = 0;
		}
		// quit if end-of-file
		if(m_nChars == 0) {_eof=true; break;}
		c = m_ibuf[m_nextout++];
		if(c == '\r' || c == '\n')
		// End of string is detected, so append '\n' if
		// lpsz is not full. and stop.
		{
			if(m_ibuf[m_nextout] == '\n' && (m_nextout < _MAX_BUFSIZE))
				m_nextout++;
			if(i < nMax-1)
				lpsz[i++] = '\n';
			break;
		}
		lpsz[i] = c;
	}
	// NULL terminate the string
	lpsz[i] = 0;
	return i == 0 ? NULL : lpsz;
}

char* CStudioFile::ReadString(char* lpsz, UINT nMax)
// Reads text data into a buffer, up to a limit of nMax -1 characters, 
// from the file associated with the CStdioFile object. 
// Reading is stopped by the first newline character. If, 
// in that case, fewer than nMax-1 characters have been read,
// a newline character ('\n') is stored in the buffer. A null character
// (‘\0’) is appended in either case. 
{
	char c;
	char *bufptr = (char *)m_ibuf;
	int bufsize = _MAX_BUFSIZE * sizeof(wchar_t);
	UINT i;
	for(i = 0; i < nMax-1; i++)
	{
		// If the buffer is empty or all data has been
		// returned, get more data from the file.
		if(m_nextout == m_nChars)
		{
			ReadFile(f, m_ibuf, _MAX_BUFSIZE*sizeof(char), &m_nChars, false);
			m_nextout = 0;
		}
		// quit if end-of-file
		if(m_nChars == 0) {_eof=true; break;}
		c = bufptr[m_nextout++];
		if(c == '\r' || c == '\n')
		// End of string is detected, so append '\n' if
		// lpsz is not full. and stop.
		{
			if( (bufptr[m_nextout] == '\n') && (m_nextout < bufsize))
				m_nextout++;
			if(i < nMax-1)
				lpsz[i++] = '\n';
			break;
		}
		lpsz[i] = c;
	}
	// NULL terminate the string
	lpsz[i] = 0;
	return i == 0 ? NULL : lpsz;
}

/*
BOOL CStudioFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags,
        CFileException* pError)
// Remove the CFile::typeText if it exists in nOpenFlags
// because this will cause an ASSERT in the base class.
{
	if(nOpenFlags & CFile::typeText)
		nOpenFlags ^= CFile::typeText;
	return CFile::Open(lpszFileName,nOpenFlags,pError);
}

void CStudioFile::WriteString(wchar_t* lpsz)
// unbuffered write.  Appends \r\n to the end of the string
{
	ASSERT(lpsz);
	if(lpsz != NULL)
		Write(lpsz, wcslen(lpsz) * sizeof(TCHAR));
	Write(L"\r\n", 2*sizeof(TCHAR));

}

void CStudioFile::WriteString(char* lpsz)
// unbuffered write.  Appends \r\n to the end of the string
{
	ASSERT(lpsz);
	if(lpsz != NULL)
		Write(lpsz, strlen(lpsz));
	Write("\r\n", 2);

}



BOOL CStudioFile::ReadString(CString& rString, bool bIsUnicode)
// Read a line into a CString object.  The default is to use UNICODE file
// format.  This function returns FALSE when end-of-file is reached, or
// TRUE is not at the end-of-file.
{
	BOOL rval;
	bool beof = false;
	wchar_t buf[_MAX_BUFSIZE];
	if(bIsUnicode)
	{
		// The file contains 16-bit UNICODE strings.
		wchar_t* ptr = ReadString(buf,_MAX_BUFSIZE);
		if(ptr == NULL) 
			rval = FALSE;
		else
		{
			rString = buf;
			rval = TRUE;
		}

	}
	else
	{
		char* ptr = ReadString((char*)buf,_MAX_BUFSIZE * sizeof(wchar_t));
		if(ptr == NULL) 
			rval = FALSE;
		else
		{
			rString = (char*)buf;
			rval = TRUE;
		}

	}
	return rval;
}

*/

// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //


