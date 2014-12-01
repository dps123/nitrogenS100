#include "stdafx.h"
#include "main.h"
#include "StudioFile.h"

CPlaylist::CPlaylist() {
	Count = 0;
	Index = 0;
	ShuffleIndex = 0;
	Data = NULL;
	IndexAlbumArtFilename[0] = 0;
}

void CPlaylist::ClearDelList() {

	for (int i = 0; i < Count; i++) {
		Data[i].DelMark = false;
	}

}

void CPlaylist::Swap(int i1, int i2) {

	PLAYLISTENTRY temp = Data[i1];
	Data[i1] = Data[i2];
	Data[i2] = temp;

	if (i1 == player()->PlayingIndex) {
		player()->PlayingIndex = i2;
	} else if (i2 == player()->PlayingIndex) {
		player()->PlayingIndex = i1;
	}

	if (i1 == Index) {
		player()->ChangePlaylistIndex(i2, false);
	} else if (i2 == Index) {
		player()->ChangePlaylistIndex(i1, false);
	}

}

void CPlaylist::RebuildShuffle() {

	if ((Count > 0) && (player()->lpConfig->cf.Shuffle)) {

		for (int i = 0; i < Count; i++) {
			Data[i].ShuffleIndex = -1;
		}

		Data[Index].ShuffleIndex = 0;         // The playlist index is the first shuffle item

		for (int i = 1; i < Count; i++) {

			int rnd = rand() % Count;
			
			while (Data[rnd].ShuffleIndex != -1) {
				rnd = rand() % Count;
			}

			Data[rnd].ShuffleIndex = i;
		}
		
		ShuffleIndex = 0;
	}

	player()->UpdatePrevNextToggle();
}

int CPlaylist::MoveTo(int ifrom, int ito, bool before) {

	if ((ifrom != ito) && (ifrom > -1) && (ito > -1) && (ifrom < Count) && (ito < Count)) {
		
		int result = -1;
		int newIndex = Index;
		int newPlayingIndex = player()->PlayingIndex;

		PLAYLISTENTRY * buf = new PLAYLISTENTRY[Count];
		
		int x = 0;
		for (int i = 0; i < Count; i++) {
		
			if (i != ifrom) {

				if ((i == ito) && before) {
					buf[x] = Data[ifrom];
					result = x;
					if (Index == ifrom) {
						newIndex = x;
					}
					if (player()->PlayingIndex == ifrom) {
						newPlayingIndex = x;
					}
					x++;
				}
				buf[x] = Data[i];
				if (Index == i) {
					newIndex = x;
				}
				if (player()->PlayingIndex == i) {
					newPlayingIndex = x;
				}
				x++;
				if ((i == ito) && !before) {
					buf[x] = Data[ifrom];
					result = x;
					if (Index == ifrom) {
						newIndex = x;
					}
					if (player()->PlayingIndex == ifrom) {
						newPlayingIndex = x;
					}
					x++;
				}

			}
		}
		delete [] Data;
		Data = buf;
		player()->PlayingIndex = newPlayingIndex;
		player()->ChangePlaylistIndex(newIndex, false);
		return result;
	}
	return -1;
}

void CPlaylist::DeleteMarked() {
	
	int newCnt = 0;
	
	for (int i = 0; i < Count; i++) {
		if (Data[i].DelMark == false) {
			newCnt++;
		}
	}

	PLAYLISTENTRY * newData = NULL;

	if (newCnt > 0) {
		newData = new PLAYLISTENTRY[newCnt];
		int x = 0;
		int newPlayingIndex = player()->PlayingIndex;
		int newIndex = Index;

		for (int i = 0; i < Count; i++) {
			if (Data[i].DelMark == false) {
				wcscpy(newData[x].FileName, Data[i].FileName);
				newData[x].ShuffleIndex = 0;
				if (Index == i) {
					newIndex = x;
				}
				if (player()->PlayingIndex == i) {
					newPlayingIndex = x;
				}
				x++;
			} else if (player()->PlayingIndex == i) {
				player()->StopAndClose();
				player()->PlayingIndex = -1;
				player()->PlayingSongDuration = 0;
				player()->PlayingSongPos = 0;
			}
		}

		FreeData();

		Data = newData;
		Count = newCnt;
		if (player()->PlayingIndex != -1) {
			player()->PlayingIndex = newPlayingIndex;
		}
		player()->ChangePlaylistIndex(max(min(newIndex, Count-1), 0), false);
		
	} else {
		
		Count = 0;
		player()->StopAndClose();
		player()->PlayingIndex = -1;
		player()->PlayingSongDuration = 0;
		player()->PlayingSongPos = 0;
		player()->ChangePlaylistIndex(0, false);
		FreeData();

	}

}

void CPlaylist::Clear() {

	Count = 0;
	player()->StopAndClose();
	player()->PlayingIndex = -1;
	player()->PlayingSongDuration = 0;
	player()->PlayingSongPos = 0;
	player()->ChangePlaylistIndex(0, false);
	FreeData();

}

bool CPlaylist::SaveToFile(LPWSTR cFileName, bool Playing, int PlayingIndex, DWORD CurrentPos) {

	DWORD n;
	HANDLE f = CreateFile(cFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	char buf[5] = "NPLX";
	WriteFile(f, &buf, 4, &n, false);
	WriteFile(f, &Count, 2, &n, false);
	WriteFile(f, &Index, 2, &n, false);

	WriteFile(f, &Playing, 1, &n, false);
	
	if (Playing) {

		WriteFile(f, &PlayingIndex, 2, &n, false);
		WriteFile(f, &CurrentPos, 4, &n, false);
	
	}

	for (int i = 0; i < Count; i++) {
		int len = wcslen(Data[i].FileName);
		WriteFile(f, &len, 2, &n, false);
		WriteFile(f, Data[i].FileName, len*sizeof(wchar_t), &n, false);
	}
	CloseHandle(f);
	return true;

}

void CPlaylist::FreeData() {
	if (Data != NULL) {
		delete [] Data;
	}
	Data = NULL;
}

int CPlaylist::GetFileIndex(LPWSTR cFileName) {

	for (int i = 0; i < Count; i++) {
		if (strCompare(Data[i].FileName, cFileName) == 0) {
			return i;
		}
	}
	return 0;
}

bool CPlaylist::LoadFromFileNPL(LPWSTR cFileName) {
	HANDLE f = CreateFile(cFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (f == INVALID_HANDLE_VALUE) {
		return false;
	}

	DWORD n;
	char buf[5];
	buf[4] = 0;
	
	if (!ReadFile(f, &buf, 4, &n, false)) {
		CloseHandle(f);
		return false;
	}

	if (strcmp("NPLX", (char*)&buf) != 0) {
		CloseHandle(f);
		return false;
	}

	Count = 0;
	ReadFile(f, &Count, 2, &n, false);
	
	if (Count < 0) {
		CloseHandle(f);
		return false;
	}
	
	if (Count == 0) {
		CloseHandle(f);
		return true;
	}

	int NewIndex = 0;
	ReadFile(f, &NewIndex, 2, &n, false);

	bool flag;
	ReadFile(f, &flag, 1, &n, false);

	int PlayingIndex = 0;
	DWORD CurrentPos = 0;

	if (flag) {

		ReadFile(f, &PlayingIndex, 2, &n, false);
		ReadFile(f, &CurrentPos, 4, &n, false);

	}

	Data = new PLAYLISTENTRY[Count];
	
	for (int i = 0; i < Count; i++) {
					
		int len = 0;
		ReadFile(f, &len, 2, &n, false);
		if (len <= 0 || len >= MAX_PATH) {
			Clear();
			CloseHandle(f);
			return false;
		}
		
		if (!ReadFile(f, Data[i].FileName, len*sizeof(wchar_t), &n, false)) {
			Clear();
			CloseHandle(f);
			return false;

		}
		Data[i].FileName[len] = 0;
	
	}

	if (player()->lpConfig->cf.RememberPos && flag) {

		player()->ChangePlaylistIndex(PlayingIndex, false);
		player()->PlayingIndex = PlayingIndex;
		player()->PlayingSongPos = CurrentPos;
	
	} else {
	
		player()->ChangePlaylistIndex(NewIndex, false);
		player()->PlayingIndex = NewIndex;
		player()->PlayingSongPos = 0;

	}

	CloseHandle(f);
	return true;
}

bool CPlaylist::LoadFromFilePLS(LPWSTR cFileName) {

	CStudioFile* file = new CStudioFile;

	if (!file->Open(cFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) {
		free(file);
		return false;
	}

	wchar_t dirName[MAX_PATH];
	ExtractFilePath(dirName, cFileName);

	Data = new PLAYLISTENTRY[Count];
	wchar_t buf[MAX_PATH];
	wchar_t FileName[MAX_PATH];
	char cbuf[MAX_PATH];

	do {
		file->ReadString(cbuf,MAX_PATH);
		MultiByteToWideChar(CP_UTF8, 0, cbuf, -1, buf, MAX_PATH);
		removeEndline(buf);
		if (wcsncmp(buf,L"File",4)==0) {
			int k=4;
			while (buf[k]!=L'=' && buf[k]!=0){
				k++;
			}
			if (buf[k]!=0) k++;
			if (buf[k]!=L'\\') {
				wsprintf(FileName, L"%s%s", dirName, &buf[k]);
			} else {
				wcscpy(FileName, &buf[k]);
			}
			AppendFile(FileName);
		}
	} while(!file->isEOF());

	free(file);
	return true;
}

bool CPlaylist::LoadFromFileM3U(LPWSTR cFileName) {

	CStudioFile* file = new CStudioFile;

	if (!file->Open(cFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) {
		free(file);
		return false;
	}

	wchar_t dirName[MAX_PATH];
	ExtractFilePath(dirName, cFileName);

	Data = new PLAYLISTENTRY[Count];

	wchar_t buf[MAX_PATH];
	wchar_t FileName[MAX_PATH];
	char cbuf[MAX_PATH];

	do {
		file->ReadString(cbuf,MAX_PATH);
		MultiByteToWideChar(CP_UTF8, 0, cbuf, -1, buf, MAX_PATH);
		removeEndline(buf);
		if (wcslen(buf)>0 && buf[0]!=L'#') {
			if (buf[0]!=L'\\') {
				wsprintf(FileName, L"%s%s", dirName, buf);
			} else {
				wcscpy(FileName, buf);
			}
			AppendFile(FileName);
		}
	} while(!file->isEOF());

	free(file);
	return true;


}

bool CPlaylist::LoadFromFile(LPWSTR cFileName) {
	
	Clear();
	if (CheckFileExt(cFileName, L".npl")) {
		if (!LoadFromFileNPL(cFileName)) {
			player()->diskWaitFlag=-1;
			return false;
		}
	} else if (CheckFileExt(cFileName, L".pls")){
		if (!LoadFromFilePLS(cFileName)) {
			player()->diskWaitFlag=-1;
			return false;
		}
	} else if (CheckFileExt(cFileName, L".m3u")|| CheckFileExt(cFileName, L".m3u8")){
		if (!LoadFromFileM3U(cFileName)) {
			player()->diskWaitFlag=-1;
			return false;
		}
	} else {
		player()->diskWaitFlag=-1;
		return false;
	}

	ReadSongDuration();

	return true;
}

void CPlaylist::ReadSongDuration() {

	player()->PlayingSongDuration = 0;
	if (Index >= 0 && Index < Count) {
		if (FileExists(Data[Index].FileName)){
			HANDLE bMap = MAP_Initialize();
			if (MAP_Open(bMap, Data[Index].FileName)) {
				player()->PlayingSongDuration = MAP_GetDuration(bMap)/1000;
				MAP_Close(bMap);
			}
			MAP_Uninitialize(bMap);
		}
	}

}
void CPlaylist::AppendPlaylist(CPlaylist * src) {

	if (Count+src->Count > 0) {
		PLAYLISTENTRY * buf = new PLAYLISTENTRY[Count+src->Count];
		for (int i = 0; i < Count; i++) {
			wcscpy(buf[i].FileName, Data[i].FileName);
			buf[i].ShuffleIndex = 0;
		}
		for (int i = 0; i < src->Count; i++) {
			wcscpy(buf[i+Count].FileName, src->Data[i].FileName);
			buf[i+Count].ShuffleIndex = 0;
		}
		FreeData();
		Data = buf;
		Count = Count+src->Count;
	} else {
		FreeData();
		Count = 0;
		Index = 0;
	}

}

int CPlaylist::AppendFile(LPWSTR FileName) {
	
	PLAYLISTENTRY * buf = new PLAYLISTENTRY[Count+1];
	for (int i = 0; i < Count; i++) {
		wcscpy(buf[i].FileName, Data[i].FileName);
		buf[i].ShuffleIndex = 0;
	}
	wcscpy(buf[Count].FileName, FileName);
	buf[Count].ShuffleIndex = 0;
	FreeData();
	Data = buf;
	Count++;
	return Count-1;
	
}

void CPlaylist::AppendSearch(LPWSTR search) {
	
	int cnt = GetFileCount(search);
	wchar_t lpPath[MAX_PATH];
	ExtractFilePath(lpPath, search);
	if (cnt > 0) {
		PLAYLISTENTRY * buf = new PLAYLISTENTRY[Count+cnt];
		for (int i = 0; i < Count; i++) {
			wcscpy(buf[i].FileName, Data[i].FileName);
			buf[i].ShuffleIndex = 0;
		}
		
		WIN32_FIND_DATA findData;
		HANDLE fileHandle = FindFirstFile(search, &findData);

		if (fileHandle != INVALID_HANDLE_VALUE) {
		
			int i = 0;
			do {
				wsprintf(buf[Count+i].FileName, L"%s%s", lpPath, findData.cFileName);
				buf[Count+i].ShuffleIndex = 0;
				i++;
			} while (FindNextFile(fileHandle, &findData));
	
			FindClose(fileHandle);
		
		}

		FreeData();
		Data = buf;
		Count = Count+cnt;
	}

}

void CPlaylist::Sort(int start, int end) {
	for (int i = start; i < end; i++) {
		for (int x = i+1; x < end; x++) {
			if (strCompare(Data[i].FileName, Data[x].FileName) > 0) {
				PLAYLISTENTRY buf = Data[i];
				Data[i] = Data[x];
				Data[x] = buf;
			}
		}
	}
}

void CPlaylist::AppendDirectory(LPWSTR Path) {
	
	int pcnt = Count;

	for (int i = 0; i < player()->ExtCount; i++) {

		wchar_t search[MAX_PATH];
		wsprintf(search, L"%s*.%s", Path, player()->ExtNames[i]);
		AppendSearch(search);

	}

	if (pcnt < Count) {
		Sort(pcnt, Count);
	}
}

void CPlaylist::AppendDirectoryRecursive(LPWSTR Path) {
	AppendDirectory(Path);
	WIN32_FIND_DATA findData;
	wchar_t search[MAX_PATH];
	wsprintf(search, L"%s*", Path);
	HANDLE fileHandle = FindFirstFile(search, &findData);
	if (fileHandle != INVALID_HANDLE_VALUE) {
		do {
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				wchar_t s2[MAX_PATH];
				wsprintf(s2, L"%s%s\\", Path, findData.cFileName);
				AppendDirectoryRecursive(s2);
			}
		} while (FindNextFile(fileHandle, &findData));
		FindClose(fileHandle);
	}
}

CPlaylist::~CPlaylist() {

	FreeData();

}
