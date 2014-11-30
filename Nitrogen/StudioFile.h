//////////////////////////////////////////////////////////////////////
//
// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File Name: StudioFile.h
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

#if !defined(AFX_STUDIOFILE_H__FA17FC8C_296D_4E53_9BD9_54A071E7F724__INCLUDED_)
#define AFX_STUDIOFILE_H__FA17FC8C_296D_4E53_9BD9_54A071E7F724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CStudioFile
{

public:
// Constructors
    CStudioFile();
//	CStudioFile(HANDLE pOpenStream);
//    CStudioFile(LPCTSTR lpszFileName, UINT nOpenFlags) {}
    virtual ~CStudioFile();

                        // m_hFile from base class is _fileno(m_pStream)

// Operations
    //virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);
	virtual BOOL CStudioFile::Open(
							LPCWSTR lpFileName,
							DWORD dwDesiredAccess,
							DWORD dwShareMode,
							LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							DWORD dwCreationDisposition,
							DWORD dwFlagsAndAttributes,
							HANDLE hTemplateFile);
	//virtual void WriteString(wchar_t* lpsz);
	//virtual void WriteString(char* lpsz);
	virtual wchar_t* ReadString(wchar_t* lpsz, UINT nMax);
	virtual char* ReadString(char* lpsz, UINT nMax);
	//virtual BOOL ReadString(CString& rString, bool bIsUnicode = true);
	virtual BOOL isEOF();

// Implementation
public:
//	virtual DWORD GetPosition() const;
//	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
//	CFileException* pError = NULL);
//	virtual UINT Read(void* lpBuf, UINT nCount);
//	virtual void Write(const void* lpBuf, UINT nCount);
//	virtual LONG Seek(LONG lOff, UINT nFrom);
//	virtual void Abort();
//	virtual void Flush();
//	virtual void Close();
protected:
	HANDLE f; // File handle
	#define _MAX_BUFSIZE 255
	wchar_t	m_ibuf[_MAX_BUFSIZE];	// input buffer
	DWORD	m_nChars;		// number of characters actaully in the buffer
	int		m_nextout;		// current position in m_ibuf
	BOOL _eof; // End of file
};


#endif // !defined(AFX_STUDIOFILE_H__FA17FC8C_296D_4E53_9BD9_54A071E7F724__INCLUDED_)
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
