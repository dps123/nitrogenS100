#include <windows.h>
#include "maplay.h"
#include "helper.h"
#include "player.h"

typedef struct _PLUGIN_INFO {
	HMODULE				hModule;
	MAP_DEC_PLUGIN*		pPlugIn;
	DWORD				dwFunc;
} PLUGIN_INFO;

void CPlayer::PlugInLoad()
{
	BOOL fLoad;
	HMODULE hModule;
	WIN32_FIND_DATA	wfd;
	TCHAR szPath[MAX_PATH];
	TCHAR szModule[MAX_PATH];
	PLUGIN_INFO* pInfo;
	MAP_DEC_PLUGIN* pPlugIn;
	MAP_DEC_PLUGIN* (WINAPI *pmapGetDecoder)();

	m_nFilePlugIn = -1;
	m_nStreamingPlugIn = -1;
	m_nPlugInBps = 0;

	GetModuleFileName(NULL, szModule, MAX_PATH);
	LPTSTR p = _tcsrchr(szModule, _T('\\'));
	if (p) *p = NULL;

	wsprintf(szPath, _T("%s\\Nitrogen\\Plugins\\*.dll"), szModule);
	
	HANDLE hFind = FindFirstFile(szPath, &wfd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			fLoad = FALSE;
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				wsprintf(szPath, _T("%s\\Nitrogen\\Plugins\\%s"), szModule, wfd.cFileName);
				hModule = LoadLibrary(szPath);
				if (hModule) {
#ifdef _WIN32_WCE
					(FARPROC&)pmapGetDecoder = GetProcAddress(hModule, _T("mapGetDecoder"));
#else
					(FARPROC&)pmapGetDecoder = GetProcAddress(hModule, "mapGetDecoder");
#endif
					if (pmapGetDecoder) {
						pPlugIn = pmapGetDecoder();
						if (pPlugIn && pPlugIn->dwVersion == PLUGIN_DEC_VER && 
							pPlugIn->Init && pPlugIn->Quit && pPlugIn->GetPluginName && 
							pPlugIn->SetEqualizer && pPlugIn->ShowConfigDlg &&
							pPlugIn->GetFileExtCount && pPlugIn->GetFileExt &&
							pPlugIn->IsValidFile && 
							pPlugIn->OpenFile && pPlugIn->SeekFile &&
							pPlugIn->StartDecodeFile && pPlugIn->DecodeFile &&
							pPlugIn->StopDecodeFile && pPlugIn->CloseFile &&
							pPlugIn->GetTag && pPlugIn->GetFileTag &&
							pPlugIn->OpenStreaming && pPlugIn->DecodeStreaming && 
							pPlugIn->CloseStreaming && pPlugIn->dwChar == sizeof(TCHAR)) {

							pPlugIn->Init();

							pInfo = new PLUGIN_INFO;
							pInfo->hModule = hModule;
							pInfo->pPlugIn = pPlugIn;
							pInfo->dwFunc = pPlugIn->GetFunc();
							m_PlugInInfo.Add((DWORD)pInfo);
							
							fLoad = TRUE;	
						}
					}
					if (!fLoad) FreeLibrary(hModule);
				}
			}
		}
		while (FindNextFile(hFind, &wfd));
		FindClose(hFind);
	}
}

void CPlayer::PlugInFree()
{
	PLUGIN_INFO* pInfo;
	while (!m_PlugInInfo.IsEmpty()) {
		pInfo = (PLUGIN_INFO*)m_PlugInInfo.RemoveAt(0);
		pInfo->pPlugIn->Quit();
		FreeLibrary(pInfo->hModule);
		delete pInfo;
	}
}

BOOL CPlayer::PlugInIsValidFile(LPCTSTR pszFile)
{
	PLUGIN_INFO* pInfo;
	int nCount = m_PlugInInfo.GetCount();
	for (int i = 0; i < nCount; i++) {
		pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(i);
				
		if (pInfo->dwFunc & PLUGIN_FUNC_DECFILE) {
			if (pInfo->pPlugIn->IsValidFile(pszFile))
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CPlayer::PlugInGetId3TagFile(LPCTSTR pszFile, ID3TAGV1* pTag)
{
	PLUGIN_INFO* pInfo;
	int nCount = m_PlugInInfo.GetCount();
	for (int i = 0; i < nCount; i++) {
		pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(i);
				
		if (pInfo->dwFunc & PLUGIN_FUNC_FILETAG) {
			MAP_PLUGIN_FILETAG tag = {0};
			if (pInfo->pPlugIn->GetFileTag(pszFile, &tag)) {

				pTag->nYear = tag.nYear;
				pTag->nTrackNum = tag.nTrackNum;
				_tcscpy(pTag->szGenre, tag.szGenre);
				_tcscpy(pTag->szAlbum, tag.szAlbum);
				_tcscpy(pTag->szArtist, tag.szArtist);
				_tcscpy(pTag->szComment, tag.szComment);
				_tcscpy(pTag->szTrack, tag.szTrack);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CPlayer::PlugInOpenFile(LPCTSTR pszFile)
{
	PLUGIN_INFO* pInfo;
	MAP_PLUGIN_FILE_INFO info;
	int nCount = m_PlugInInfo.GetCount();
	for (int i = 0; i < nCount; i++) {
		pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(i);
		if (pInfo->dwFunc & PLUGIN_FUNC_DECFILE) {
			if (pInfo->pPlugIn->OpenFile(pszFile, &info)) {
				// Skipping too audiophile files :) We can't reproduce this on ppcflac 2007/our device
				if (info.nBitsPerSample > 16 || info.nSampleRate > 44100){
					//nSamplingRate = 44100
					//44100
					//					LPWSTR sChannels;
					//					wsprintf(sChannels, L"%d", info.nBitsPerSample);
					pInfo->pPlugIn->CloseFile();

					//TCHAR  s[100]=_T("Unable to open file: ");
					//					MessageBox(NULL, _tcscat(s, pszFile), L"ERROR", MB_ICONERROR|MB_OK);
					return false;
				}else{
				    memset(&m_Info, 0, sizeof(m_Info));
					m_Info.nChannels = info.nChannels;
					m_Info.nSamplingRate = info.nSampleRate;
					m_Info.nBitRate = info.nAvgBitrate;
					m_nPlugInBps = info.nBitsPerSample;
					m_Info.nFrameSize = 0;
					m_Info.nSamplesPerFrame = 0;
					m_nDuration = (int)(((double)info.nDuration * m_Info.nSamplingRate) / 1000);

					m_nFilePlugIn = i;
					m_fOpen = OPEN_PLUGIN;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CPlayer::PlugInSeekFile(long lTime)
{
	if (m_nFilePlugIn == -1)
		return FALSE;

	PLUGIN_INFO* pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(m_nFilePlugIn);
	if (!(pInfo->dwFunc & PLUGIN_FUNC_SEEKFILE))
		return FALSE;

	long lSeek = pInfo->pPlugIn->SeekFile(lTime);
	if (lSeek == -1)
		return FALSE;

	m_fSeek = TRUE;
	m_nSeek = (int)((double)lSeek * m_Info.nSamplingRate / 1000);
	m_Output.Reset();

	return TRUE;
}

DWORD CPlayer::PlugInPlayerThread()
{
	if (m_nFilePlugIn == -1)
		return RET_ERROR;

	PLUGIN_INFO* pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(m_nFilePlugIn);

	if (!pInfo->pPlugIn->StartDecodeFile())
		return RET_ERROR;

	int nRet;
	while (TRUE) {
		if (m_fStop)
			return RET_STOP;

		if (!m_pOutHdr)
			m_pOutHdr = m_Output.GetBuffer();

		{
			CAutoLock lock(&m_csecThread);
			if (m_fSeek) {
				if (m_Status == MAP_STATUS_PLAY)
					m_fPlay = TRUE;

				m_Reverb.Reset();
				m_Echo.Reset();
				m_BassBoost.Reset();
				m_3DChorus.Reset();
				m_Output.Reset();
				m_fSeek = FALSE;
				m_pOutHdr = NULL;
				continue;
			}
		
			m_pOutHdr->dwBytesRecorded = 0;
			m_pOutHdr->dwBufferLength = m_cbOutBuf;
			nRet = pInfo->pPlugIn->DecodeFile(m_pOutHdr);
		}

		if (nRet == PLUGIN_RET_ERROR)
			return RET_ERROR;
		
		if (!(pInfo->dwFunc & PLUGIN_FUNC_EQ))
			Preamp((LPBYTE)m_pOutHdr->lpData, m_pOutHdr->dwBytesRecorded);

		OutputBuffer(m_pOutHdr, m_pOutHdr->dwBytesRecorded);
		m_pOutHdr = NULL;

		if (nRet == PLUGIN_RET_EOF)
			return RET_EOF;

		if (m_fSuppress)
			return RET_EOF;
	}
}

void CPlayer::PlugInStop()
{
	if (m_nFilePlugIn == -1)
		return;

	PLUGIN_INFO* pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(m_nFilePlugIn);
	pInfo->pPlugIn->StopDecodeFile();
}

void CPlayer::PlugInClose()
{
	if (m_nFilePlugIn == -1)
		return;

	PLUGIN_INFO* pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(m_nFilePlugIn);
	pInfo->pPlugIn->CloseFile();
	m_nFilePlugIn = -1;
	m_nPlugInBps = 0;
}

void CPlayer::PlugInSetEqualizer()
{
	int i;
	PLUGIN_INFO* pInfo;

	MAP_PLUGIN_EQ eq;
	eq.bEnable = m_Equalizer.fEnable;
	eq.nPreamp = m_Equalizer.preamp;

	for (i = 0; i < 10; i++)
		eq.nEq[i] = m_Equalizer.data[i];

	int nCount = m_PlugInInfo.GetCount();
	for (i = 0; i < nCount; i++) {
		pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(i);

		if (pInfo->dwFunc & PLUGIN_FUNC_EQ)
			pInfo->pPlugIn->SetEqualizer(&eq);
	}
}

BOOL CPlayer::PlugInGetId3Tag(ID3TAGV1* pTag)
{
	if (m_nFilePlugIn == -1)
		return FALSE;

	PLUGIN_INFO* pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(m_nFilePlugIn);
	if (pInfo->dwFunc & PLUGIN_FUNC_FILETAG) {
		MAP_PLUGIN_FILETAG tag = {0};
		BOOL fRet = pInfo->pPlugIn->GetTag(&tag);

		pTag->nYear = tag.nYear;
		pTag->nTrackNum = tag.nTrackNum;
		_tcscpy(pTag->szGenre, tag.szGenre);
		_tcscpy(pTag->szAlbum, tag.szAlbum);
		_tcscpy(pTag->szArtist, tag.szArtist);
		_tcscpy(pTag->szComment, tag.szComment);
		_tcscpy(pTag->szTrack, tag.szTrack);

		return fRet;
	}

	return FALSE;
}

int CPlayer::PlugInGetCount()
{
	return m_PlugInInfo.GetCount();
}

MAP_DEC_PLUGIN* CPlayer::PlugInGetInterface(int nPlugIn)
{
	if (nPlugIn >= PlugInGetCount())
		return NULL;

	PLUGIN_INFO* pInfo = (PLUGIN_INFO*)m_PlugInInfo.GetAt(nPlugIn);
	return pInfo->pPlugIn;
}