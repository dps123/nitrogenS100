// utils.c - Defines common-use functions

#include "stdafx.h"
#include "main.h"

COLORREF MixColors(COLORREF c1, COLORREF c2, double grad) {
	int r = GetRValue(c1)+(int)((double)(GetRValue(c2)-GetRValue(c1))*grad);
	int g = GetGValue(c1)+(int)((double)(GetGValue(c2)-GetGValue(c1))*grad);
	int b = GetBValue(c1)+(int)((double)(GetBValue(c2)-GetBValue(c1))*grad);
	return RGB(r, g, b);
}

void ClearWbrList(WBRLIST * wbr) {
	if (wbr->Data != NULL) {
		delete [] wbr->Data;
		wbr->Data = NULL;
	}
}

void SetWbrItem(WBRITEM * wItem, LPWSTR Text, int IconIndex, bool Checked, bool Separator, bool Arrow) {
	wItem->Checked = Checked;
	wItem->IconIndex = IconIndex;
	wcscpy(wItem->Text, Text);
	wItem->TextLength = wcslen(Text);
	wItem->Separator = Separator;
	wItem->Arrow = Arrow;
}

void SetSoftkeyText(HWND hWndMenuBar, int softKey, LPWSTR newText) {
	TBBUTTONINFO tbbi = {0};
	tbbi.cbSize = sizeof(tbbi);
	tbbi.dwMask = TBIF_TEXT;
	tbbi.pszText = newText;
	SendMessage(hWndMenuBar, TB_SETBUTTONINFO, softKey, (LPARAM)&tbbi);
}

bool CheckFileExt(LPWSTR path, LPWSTR ext) {
	int l1 = wcslen(path);
	int l2 = wcslen(ext);
	if (l1-l2 < 0) {
		return false;
	}
	for (int i = l1-l2; i < l1; i++) {
		int ch = path[i];
		if ((65 <= ch) && (ch <= 90)) {  // Performs case insensitive comparisons
			ch += 32;
		}
		if (ch != ext[i-l1+l2]) {
			return false;
		}
	}
	return true;
}
bool ByteEqual(LPBYTE b1, LPBYTE b2, int MaxLen) {
	int i = 0;
	bool res = true;
	while ((i < MaxLen) && (b1[i] != 0) && (b2[i] != 0)) {
		if (b1[i] != b2[i]) {
			res = false;
			break;
		}
		i++;
	}
	return res;
}
bool LoadJPEG(LPWSTR fName, XBITMAP * xbit, int wid, int hei) {
	
	HANDLE fileHandle;
	WIN32_FIND_DATA findData;
	fileHandle = FindFirstFile(fName, &findData);

	if (fileHandle == INVALID_HANDLE_VALUE) {
		return false;
	}
	
	FindClose(fileHandle);
	if (findData.nFileSizeLow == 0) {
		return false;
	}

	FILE * f = _tfopen(fName, L"rb");
	if (f == NULL) {
		return false;
	}

	jpeg_decompress_struct cInfo;
	jpeg_error_mgr jErr;

	cInfo.err = jpeg_std_error(&jErr);
	jpeg_create_decompress(&cInfo);
	jpeg_stdio_src(&cInfo, f);
	if (jpeg_read_header(&cInfo, true) != JPEG_HEADER_OK || !jpeg_start_decompress(&cInfo)) {
		return false;
	}

	int oInputRowSize = ((cInfo.output_width*cInfo.output_components+3)/4)*4;
	int oOutputRowSize = ((wid*3+3)/4)*4;

	JSAMPARRAY rowBuf = (*cInfo.mem->alloc_sarray) ((j_common_ptr) &cInfo, JPOOL_IMAGE, oInputRowSize, 1);
	BYTE * buf = new BYTE[hei*oOutputRowSize];

	jpeg_read_scanlines(&cInfo, rowBuf, 1);
	
	for (int y = 0; y < hei; y++) {

		int pix = (y*cInfo.output_height/hei);
		while ((cInfo.output_scanline <= pix) && (cInfo.output_scanline < cInfo.output_height)) {
			jpeg_read_scanlines(&cInfo, rowBuf, 1);
		}

		for (int x = 0; x < wid; x++) {
			int r = 0;
			int g = 0;
			int b = 0;
			for (int z = 0; z < 3; z++) {  // enhances quality (subpixels)
				int col = ( cInfo.output_width*(3*x+1*z)/(3*wid) )*3;
				if (col >= oInputRowSize) {
					col = (cInfo.output_width-1)*3;
				}
				r += (BYTE)rowBuf[0][col+2];
				g += (BYTE)rowBuf[0][col+1];
				b += (BYTE)rowBuf[0][col];
			}
			buf[(hei-y-1)*oOutputRowSize+x*3] = r/3;
			buf[(hei-y-1)*oOutputRowSize+x*3+1] = g/3;
			buf[(hei-y-1)*oOutputRowSize+x*3+2] = b/3;
		}

	}

	while (cInfo.output_scanline < cInfo.output_height) {
		jpeg_read_scanlines(&cInfo, rowBuf, 1);
	}

	xbit->Exists = true;

	BITMAPINFO inf;
	memset(&inf, 0, sizeof(inf));
	inf.bmiHeader.biSize = sizeof(inf.bmiHeader);
	inf.bmiHeader.biWidth = wid;
	inf.bmiHeader.biHeight = hei;
	inf.bmiHeader.biPlanes = 1;
	inf.bmiHeader.biBitCount = 8*cInfo.output_components;
	inf.bmiHeader.biCompression = BI_RGB;
	inf.bmiHeader.biSizeImage = oOutputRowSize*hei;
	inf.bmiHeader.biXPelsPerMeter = 3780;
	inf.bmiHeader.biYPelsPerMeter = 3780;

	xbit->hDC = CreateCompatibleDC(NULL);
	xbit->hBitmap = CreateBitmap(wid, hei, 1, inf.bmiHeader.biBitCount, NULL);
	xbit->hBitmapOld = (HBITMAP)SelectObject(xbit->hDC, xbit->hBitmap);

	SetDIBitsToDevice(xbit->hDC, 0, 0, wid, hei, 0, 0, 0, hei, buf, &inf, DIB_RGB_COLORS);

	xbit->Width = wid;
	xbit->Height = hei;

	jpeg_finish_decompress(&cInfo);
	jpeg_destroy_decompress(&cInfo);
	fclose(f);
	delete [] buf;
	return true;

}

void ExtractFilePath(LPWSTR lpOut, LPWSTR lpPath) {
	int p = -1;
	int i = 0;
	while ((lpPath[i] != 0) && (i < MAX_PATH)) {	
		if (lpPath[i] == 92) {
			p = i;
		}
		i++;
	}
	if (p == -1) {
		wcscpy(lpOut, lpPath);
	} else {
		lpOut[p+1] = 0;
		for (int x = 0; x <= p; x++) {
			lpOut[x] = lpPath[x];
		}
	}
}

void CreateXBitmap(XBITMAP * xbmp, int wid, int hei, int bits, bool exists) {
	xbmp->Exists = exists;
	xbmp->hBitmap = CreateBitmap(wid, hei, 1, bits, NULL);
	xbmp->hDC = CreateCompatibleDC(NULL);
	xbmp->hBitmapOld = (HBITMAP)SelectObject(xbmp->hDC, xbmp->hBitmap);
	xbmp->Height = hei;
	xbmp->Width = wid;
}

void DeleteXBitmap(XBITMAP * xbmp) {
	
	if (xbmp->Exists) {
		SelectObject(xbmp->hDC, xbmp->hBitmapOld);
		DeleteObject(xbmp->hBitmap);
		DeleteObject(xbmp->hDC);
		xbmp->Exists = false;
	}

}

// Creates a font based on size and weight
HFONT CreateFont(int fSize, int fWeight, bool draft) {

	LOGFONT lFont;
	memset(&lFont, 0, sizeof(LOGFONT));
	lFont.lfHeight = fSize;
	lFont.lfCharSet = DEFAULT_CHARSET;

	lFont.lfWeight = fWeight;
	lFont.lfQuality = DEFAULT_QUALITY;
	lFont.lfPitchAndFamily = FF_SWISS;
	if (player()->lpConfig->cf.ForceCleartype)  {
		lFont.lfQuality = CLEARTYPE_QUALITY;
	}
	if (draft) {
		lFont.lfQuality = DRAFT_QUALITY;
	}
	return CreateFontIndirect(&lFont);	
}

bool ReadFileBitmap(HANDLE f, XBITMAP * xbit) {
	DWORD n;

	bool exists = false;
	ReadFile(f, &exists, 1, &n, false);

	if (exists) {
		int wid = 0;
		ReadFile(f, &wid, 2, &n, false);

		int hei = 0;
		ReadFile(f, &hei, 2, &n, false);

		int len = 0;
		ReadFile(f, &len, 4, &n, false);

		xbit->Width = wid;
		xbit->Height = hei;

		BYTE * data = new BYTE[len];
		ReadFile(f, data, len, &n, false);

		xbit->hBitmap = CreateBitmap(wid, hei, 1, 16, NULL);
		xbit->hDC = CreateCompatibleDC(NULL);

		xbit->hBitmapOld = (HBITMAP)SelectObject(xbit->hDC, xbit->hBitmap);

		BITMAPINFO inf;
		memset(&inf, 0, sizeof(inf));
		inf.bmiHeader.biSize = sizeof(inf.bmiHeader);
		inf.bmiHeader.biWidth = wid;
		inf.bmiHeader.biHeight = hei;
		inf.bmiHeader.biPlanes = 1;
		inf.bmiHeader.biBitCount = 16;
		inf.bmiHeader.biCompression = BI_RGB;
		inf.bmiHeader.biSizeImage = len;//wid*hei*2;
		inf.bmiHeader.biXPelsPerMeter = 2835;
		inf.bmiHeader.biYPelsPerMeter = 2835;

		if (!SetDIBitsToDevice(xbit->hDC, 0, 0, wid, hei, 0, 0, 0, hei, data, &inf, DIB_RGB_COLORS)) {
			delete [] data;
			xbit->Exists = false;
			return false;
		}

		delete [] data;	
		xbit->Exists = true;
		return true;
	} else {
		xbit->hBitmap = 0;
		xbit->hDC = 0;
		xbit->Height = 0;
		xbit->Width = 0;
		xbit->Exists = false;
		return true;
	}
}

// Copies a string pointer to a new pointer
LPWSTR strCopy(LPWSTR src) {
	wchar_t * buf = new wchar_t[wcslen(src)+1];
	wcscpy(buf, src);
	buf[wcslen(src)] = 0;
	return buf;
}

// Compares two strings
int strCompare(LPWSTR s1, LPWSTR s2) {
	// My algorythm has shown to be faster ;)

	int l1 = wcslen(s1);
	int l2 = wcslen(s2);
	int l = min(l1, l2);

	for (int i = 0; i < l; i++) {
		int c1 = s1[i];
		if ((65 <= c1) && (c1 <= 90)) {  // Performs case insensitive comparisons
			c1 += 32;
		}
		int c2 = s2[i];
		if ((65 <= c2) && (c2 <= 90)) {  // Performs case insensitive comparisons
			c2 += 32;
		}
		if (c1 < c2) {
			return -2;
		} else if (c1 > c2) {
			return 2;
		}
	}
	if (l1 < l2) {
		return -1;
	} else if (l1 > l2) {
		return 1;
	}
	return 0;
}

bool FileSearch(LPWSTR lpOut, LPWSTR searchPath, LPWSTR searchFile, LPWSTR searchExt) {

	HANDLE fileHandle;
	WIN32_FIND_DATA findData;
	wchar_t search[MAX_PATH];
	wsprintf(search, L"%s*%s*%s", searchPath, searchFile, searchExt);
	fileHandle = FindFirstFile(search, &findData);

	if (fileHandle != INVALID_HANDLE_VALUE) {
		FindClose(fileHandle);
		wsprintf(lpOut, L"%s%s", searchPath, findData.cFileName);
		return true;
	}

	return false;

}

void ChangeFileExt(LPWSTR lpOut, LPWSTR fName, LPWSTR newExt) {

	if (wcslen(fName) >= wcslen(newExt)) {
	
		wcscpy(lpOut, fName);

		for (int i = 0; i < wcslen(newExt); i++) {
			lpOut[wcslen(fName)-wcslen(newExt)+i] = newExt[i];
		}

	} else {
		lpOut[0] = 0;
	}
}

// Gets the song info for a given filename
void GetSongInfo(SONGINFO * sInfo, LPWSTR songFile) {

	if (player()->lpConfig->cf.Id3Tags && FileExists(songFile)) {
		ID3TAGV1 pTag;
		memset(&pTag, 0, sizeof(ID3TAGV1));

		if (MAP_GetId3TagFile(player()->MAP, songFile, &pTag)) {

			if (pTag.szTrack[0] != 0 && pTag.szArtist[0] != 0 && pTag.szAlbum[0] != 0) { 
			
				sInfo->songName[MAX_TAG_STR-1] = 0;
				for (int i = 0; i < MAX_TAG_STR-1; i++) {
					sInfo->songName[i] = pTag.szTrack[i];
				}

				sInfo->songArtist[MAX_TAG_STR-1] = 0;
				for (int i = 0; i < MAX_TAG_STR-1; i++) {
					sInfo->songArtist[i] = pTag.szArtist[i];
				}

				sInfo->songAlbum[MAX_TAG_STR-1] = 0;
				for (int i = 0; i < MAX_TAG_STR-1; i++) {
					sInfo->songAlbum[i] = pTag.szAlbum[i];
				}

				return;

			}
		}
	}

	sInfo->songAlbum[0] = 0;
	int pMin = -1;
	int pExt = -1;
	int aSpc = 0;
	int bSpc = 0;
	int pBar = 0;
	int len = 0;
	while (((int)songFile[len] != 0) && (len < 260)) {
		if (songFile[len] == 45) { // Compares if the character is "-"
			pMin = len;
			aSpc = (int)(songFile[len-1] == 32); // Strips the space after the song artist
			bSpc = (int)(songFile[len+1] == 32); // Strips the space before the song name
		}
		if (songFile[len] == 92) { // Compares if the character is "\"
			pBar = len;
		}
		if (songFile[len] == 46) { // Compares if the character is "."
			pExt = len;
		}
		len++;
	}
	if ((pMin > 0) && (pBar > 0) && (pBar > pMin)) {
		pMin = -1;
	}
	if (pExt == -1 || pMin == -1) {
		sInfo->songName[min(MAX_TAG_STR-1, pExt-pBar-1)] = 0;
		for (int i = 0; i < min(MAX_TAG_STR-1, pExt-pBar-1); i++) {
			sInfo->songName[i] = songFile[pBar+1+i];
		}
		wcscpy(sInfo->songArtist, _str(STR_UNKNOWNARTIST));
	} else {
		sInfo->songArtist[min(MAX_TAG_STR-1, pMin-pBar-aSpc-1)] = 0;
		sInfo->songName[min(MAX_TAG_STR-1, pExt-pMin-bSpc-1)] = 0;
		for (int i = pBar+1; i < len; i++) {
			if (i < pMin-aSpc) {
				sInfo->songArtist[min(MAX_TAG_STR-2, i-pBar-1)] = songFile[i];
			} else if (i > pMin+bSpc && i < pExt) {
				sInfo->songName[min(MAX_TAG_STR-2, i-pMin-1-bSpc)] = songFile[i];
			}
		}
	}
}

// Gets a rect pointer based on coordinates
LPRECT getRect(int x1, int y1, int x2, int y2) {
	RECT * result = new RECT;
	result->left = x1;
	result->top = y1;
	result->right = x2;
	result->bottom = y2;
	return result;
}

void WndRepaint(HWND hWnd, bool Enable) {
	if (Enable) {
		InvalidateRect(hWnd, NULL, false);
	}
}

void ExtractFileName(LPWSTR lpOut, LPWSTR lpPath) {
	int lastPos = -1;
	for (int i = 0; i < wcslen(lpPath)-1; i++) {
		if (lpPath[i] == 92) {
			lastPos = i;
		}
	}
	if (lastPos == -1) {
		wcscpy(lpOut, lpPath);
	} else {
		lpOut[wcslen(lpPath)-lastPos-1] = 0;
		for (int i = lastPos+1; i < wcslen(lpPath); i++) {
			lpOut[i-lastPos-1] = lpPath[i];
		}
	}
}

void ExtractDirName(LPWSTR lpOut, LPWSTR lpPath) {
	if (wcslen(lpPath) == 1) {
		wcscpy(lpOut, lpPath);
	} else {
		int lastPos = -1;
		for (int i = 0; i < wcslen(lpPath)-1; i++) {
			if (lpPath[i] == 92) {
				lastPos = i;
			}
		}
		if (lastPos == -1) {
			wcscpy(lpOut, lpPath);
		} else {
			lpOut[wcslen(lpPath)-lastPos-2] = 0;
			for (int i = lastPos+1; i < wcslen(lpPath)-1; i++) {
				lpOut[i-lastPos-1] = lpPath[i];
			}
		}
	}
}

bool strExtract(LPWSTR lpOut, LPWSTR src, int ch) {
	int len = wcslen(src);
	int x = 0;
	int pos[2] = {0, 0};
	for (int i = 0; i < len; i++) {
		if (src[i] == ch) {
			pos[x] = i;
			x++;
			if (x >= 2) {
				break;
			}
		}
	}
	if (x < 2) {
		return false;
	} else {
		lpOut[pos[1]-pos[0]-1] = 0;
		for (int i = 0; i < pos[1]-pos[0]-1; i++) {
			lpOut[i] = src[i+pos[0]+1];
		}
		return true;
	}
}

int GetFileCount(LPWSTR search) {
	HANDLE fileHandle;
	WIN32_FIND_DATA findData;
	fileHandle = FindFirstFile(search, &findData);
	int cnt = 0;
	if (fileHandle != INVALID_HANDLE_VALUE) {
		cnt = 1;
		while (FindNextFile(fileHandle, &findData)) {
			cnt++;
		}
		FindClose(fileHandle);
	}
	return cnt;
}

long FileSize(LPWSTR filePath) {
	HANDLE hF = CreateFile(
		filePath, // file name
		GENERIC_READ,                // access mode
		0,                           // share mode
		NULL,                        // SD
		OPEN_EXISTING,               // how to create
		0,                           // file attributes
		NULL                         // handle to template file
		);
	long FileSize = GetFileSize(hF, NULL);
	CloseHandle(hF);
	return FileSize;
}

bool FileExists(LPWSTR search) {
	HANDLE fileHandle;
	WIN32_FIND_DATA findData;
	fileHandle = FindFirstFile(search, &findData);
	if (fileHandle != INVALID_HANDLE_VALUE) {
		FindClose(fileHandle);
		return true;
	}
	return false;
}

bool GetAlbumArtFilename(LPWSTR lpOut, LPWSTR lpSongFilename, LPWSTR lpSongAlbum) {
	
	wchar_t lpPath[MAX_PATH];
	ExtractFilePath(lpPath, lpSongFilename);
	bool aaFound = false;

	wchar_t lpArtPath[MAX_PATH];
	ChangeFileExt(lpArtPath, player()->lpPlaylist->Data[player()->lpPlaylist->Index].FileName, L".jpg");
	if (lpArtPath[0] != 0) {
		// allow only images < 500K
		if (FileExists(lpArtPath) && FileSize(lpArtPath) < 500 * 1024) {
			aaFound = true;
			wcscpy(lpOut, lpArtPath);
		}
	}

	if ((!aaFound) && (lpSongAlbum[0] != 0)) {

		wchar_t lpAlbumPath[MAX_PATH];
		if (FileSearch(lpAlbumPath, lpPath, lpSongAlbum, L".jpg")) {
			aaFound = true;
			wcscpy(lpOut, lpAlbumPath);
		}
	}
	
	if (!aaFound) {

		LPWSTR fNames[4] = {L"Folder.jpg", L"Cover.jpg", L"AlbumArt.jpg", L"AlbumArtSmall.jpg"};

		for (int i = 0; i < 4; i++) {
			wchar_t s[MAX_PATH];
			wsprintf(s, L"%s%s", lpPath, fNames[i]);
			if (FileExists(s)) {
				wcscpy(lpOut, s);
				aaFound = true;
			}
			if (aaFound) { 
				break;
			}
		}
	
	}

	if (!aaFound) {
		lpOut[0] = 0;
	}

	return aaFound;
}

void SecsToTime(LPWSTR lpOut, int secs) {
	int sc = secs % 60;
	int mn = secs / 60;
	wsprintf(lpOut, L"%02i:%02i", mn, sc);
}

// Functions based on http://social.msdn.microsoft.com/forums/en-US/vssmartdevicesnative/thread/7b0a31a8-82f1-4bb8-84bc-53032ca82c72/

HINSTANCE g_hHookApiDLL = NULL;


bool HookLoadProcs(HOOKPROCS * hookProcs) {
	
	g_hHookApiDLL = LoadLibrary(L"coredll.dll"); 
    if(g_hHookApiDLL == NULL) { 

		return false; 

	} else { 

		hookProcs->SetWindowsHookEx = (_SetWindowsHookExW)GetProcAddress(g_hHookApiDLL, _T("SetWindowsHookExW"));
		hookProcs->CallNextHookEx = (_CallNextHookEx)GetProcAddress(g_hHookApiDLL, _T("CallNextHookEx")); 
        hookProcs->UnhookWindowsHookEx = (_UnhookWindowsHookEx)GetProcAddress(g_hHookApiDLL, _T("UnhookWindowsHookEx"));
		if (hookProcs->SetWindowsHookEx == NULL || hookProcs->CallNextHookEx == NULL || hookProcs->UnhookWindowsHookEx == NULL) {
			return false;
		}

	}
	return true;
}

bool HookCloseProcs(HOOKPROCS * hookProcs) { 

	if(g_hHookApiDLL != NULL) { 

		FreeLibrary(g_hHookApiDLL); 
        g_hHookApiDLL = NULL;
		hookProcs->SetWindowsHookEx = NULL;
		hookProcs->CallNextHookEx = NULL;
		hookProcs->UnhookWindowsHookEx = NULL;

	} 
    return true; 
}
