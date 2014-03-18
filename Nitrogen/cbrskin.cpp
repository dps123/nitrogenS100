#include "stdafx.h"
#include "main.h"

CBrSkin::CBrSkin() {

	ListSkins.Data = NULL;
	ListSkins.CheckEnabled = false;
	ListSkins.OffY = 0;
	ListSkins.TitleIcon = -1;
}

bool CBrSkin::Show() {

	wcscpy(ListSkins.Title, player()->SkinsPath);
	ClearWbrList(&ListSkins);

	wchar_t skinSearch[MAX_PATH];
	wsprintf(skinSearch, L"%s*.sk2", player()->SkinsPath);

	ListSkins.Count = GetFileCount(skinSearch);
	ListSkins.ItemIndex = -1;
	
	if (ListSkins.Count > 0) {
	
		ListSkins.Data = new WBRITEM[ListSkins.Count];
	
		HANDLE fileHandle;
		WIN32_FIND_DATA findData;
		fileHandle = FindFirstFile(skinSearch, &findData);

		int x = 0;
		do {
			SetWbrItem(&ListSkins.Data[x], findData.cFileName, ICO_SSKINFILE, false, false, false);
			if (strCompare(findData.cFileName, player()->lpConfig->cf.sSkinFile) == 0) {
				ListSkins.ItemIndex = x;
			}
			x++;
		} while (FindNextFile(fileHandle, &findData));

		FindClose(fileHandle);

	} else {
		ListSkins.Data = NULL;
	}


	return player()->lpWndBrowser->Show(_str(STR_SKINSELECTOR), _str(STR_CANCEL), _str(STR_OK), &ListSkins, CBrSkinMsgProc);
}

int CBrSkin::MsgProc(int uMsg) {

	if (uMsg == WBR_LSOFTKEY) {
		
		player()->lpWndBrowser->Close();
	
	}

	if ((uMsg == WBR_RSOFTKEY) || (uMsg == WBR_SELECTED) || (uMsg == WBR_OK)) {

		if (ListSkins.ItemIndex >= 0) {

			wchar_t SkinFilename[MAX_PATH];
			wsprintf(SkinFilename, L"%s%s", player()->SkinsPath, ListSkins.Data[ListSkins.ItemIndex].Text);

			if (iLoadSkin(player()->lpWndBrowser->hWnd, SkinFilename, false)) {
		
				player()->lpWndBrowser->Close();
				
				wcscpy(player()->lpConfig->cf.sSkinFile, ListSkins.Data[ListSkins.ItemIndex].Text);
				
				for (int i = 0; i < getItemsCount(); i++) {
					if (getItem(i)->Kind == IIK_TRACKBAR) {
						iDrawTrackbar(NULL, i, true);
					}
				}
				player()->lpWndMain->UpdateAlbumArt();
				player()->lpWndMain->Repaint(PNT_FORCEFULL, 0);
			}

		}
	}
	return 0;
}

int CBrSkinMsgProc(int uMsg) {
	
	return player()->lpBrSkin->MsgProc(uMsg);

}

CBrSkin::~CBrSkin() {

	ClearWbrList(&ListSkins);
}
