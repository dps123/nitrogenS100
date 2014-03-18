#include "stdafx.h"
#include "main.h"

CBrLang::CBrLang() {

	ListLangs.Data = NULL;
	ListLangs.CheckEnabled = false;
	ListLangs.OffY = 0;
	ListLangs.TitleIcon = -1;
	NameLangs = NULL;

}

bool CBrLang::Show() {

	wcscpy(ListLangs.Title, player()->LangsPath);
	
	if (NameLangs != NULL) {
		delete [] NameLangs;
	}

	ClearWbrList(&ListLangs);

	wchar_t langSearch[MAX_PATH];
	wsprintf(langSearch, L"%s*.nlg", player()->LangsPath);

	ListLangs.Count = GetFileCount(langSearch)+1;
	ListLangs.ItemIndex = 0;
	
	NameLangs = new WPATHSTR[ListLangs.Count];
	ListLangs.Data = new WBRITEM[ListLangs.Count];
	SetWbrItem(&ListLangs.Data[0], L"English", ICO_SSKINFILE, false, false, false);
	memset(NameLangs[0], 0, sizeof(NameLangs[0]));

	HANDLE fileHandle;
	WIN32_FIND_DATA findData;
	fileHandle = FindFirstFile(langSearch, &findData);

	if (fileHandle != INVALID_HANDLE_VALUE) {

		int x = 1;
		do {
			wchar_t langFile[MAX_PATH];
			wsprintf(langFile, L"%s%s", player()->LangsPath, findData.cFileName);
			wchar_t langName[50];
			if (lang_getname(langFile, langName)) {
			
				SetWbrItem(&ListLangs.Data[x], langName, ICO_SSKINFILE, false, false, false);
			
			} else {

				SetWbrItem(&ListLangs.Data[x], findData.cFileName, ICO_SSKINFILE, false, false, false);

			}
			if (strCompare(findData.cFileName, player()->lpConfig->cf.sLangFile) == 0) {
				ListLangs.ItemIndex = x;
			}
			wcscpy(NameLangs[x], findData.cFileName); 
			x++;

		} while (FindNextFile(fileHandle, &findData));

		FindClose(fileHandle);
	}

	for (int i = 0; i < ListLangs.Count; i++) {
	
		for (int x = i+1; x < ListLangs.Count; x++) {
		
			if (strCompare(ListLangs.Data[i].Text, ListLangs.Data[x].Text) > 0) {
			
				WBRITEM tmp = ListLangs.Data[i];
				ListLangs.Data[i] = ListLangs.Data[x];
				ListLangs.Data[x] = tmp;

				if (ListLangs.ItemIndex == i) {
					ListLangs.ItemIndex = x;
				} else if (ListLangs.ItemIndex == x) {
					ListLangs.ItemIndex = i;
				}

				WPATHSTR tmp1;
				wcscpy(tmp1, NameLangs[i]);
				wcscpy(NameLangs[i], NameLangs[x]);
				wcscpy(NameLangs[x], tmp1);

			}

		}
	}
	return player()->lpWndBrowser->Show(_str(STR_LANGUAGESELECTOR), _str(STR_CANCEL), _str(STR_OK), &ListLangs, CBrLangMsgProc);
}

int CBrLang::MsgProc(int uMsg) {

	if (uMsg == WBR_LSOFTKEY) {
		
		player()->lpWndBrowser->Close();
	
	}

	if ((uMsg == WBR_RSOFTKEY) || (uMsg == WBR_SELECTED) || (uMsg == WBR_OK)) {

		if (ListLangs.ItemIndex >= 0) {

			if (NameLangs[ListLangs.ItemIndex][0] != 0) {

				wchar_t langFileName[MAX_PATH];
				wsprintf(langFileName, L"%s%s", player()->LangsPath, NameLangs[ListLangs.ItemIndex]);

				if (lang_loadfromfile(langFileName)) {
					wcscpy(player()->lpConfig->cf.sLangFile, NameLangs[ListLangs.ItemIndex]);
				} else {	
					MessageBox(player()->lpWndBrowser->hWnd, L"Could not load the selected language file. The default (English) will be used.", L"Error", MB_OK|MB_ICONERROR);
					memset(player()->lpConfig->cf.sLangFile, 0, sizeof(player()->lpConfig->cf.sLangFile));
				}
				
			} else {
			
				lang_loaddefaults();
				memset(player()->lpConfig->cf.sLangFile, 0, sizeof(player()->lpConfig->cf.sLangFile));

			}

			player()->lpWndBrowser->Close();
			player()->lpWndMain->StartMenuNames();
			player()->lpBrMedia->StartMenuNames();
			player()->lpWndMain->SetLSoftkeyText();
			SetSoftkeyText(player()->lpWndMain->hWndMenuBar, 1, _str(STR_OPTIONS));

		}
	}
	return 0;
}

int CBrLangMsgProc(int uMsg) {
	
	return player()->lpBrLang->MsgProc(uMsg);

}

CBrLang::~CBrLang() {

	if (NameLangs != NULL) {
		delete [] NameLangs;
	}

	ClearWbrList(&ListLangs);
}
