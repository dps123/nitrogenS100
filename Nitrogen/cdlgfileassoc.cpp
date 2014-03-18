#include "stdafx.h"
#include "main.h"

void CDlgFileAssoc::Execute() {

	DialogBox(player()->hInstance, MAKEINTRESOURCE(DLG_FILEASSOC), player()->lpWndMain->hWnd, (DLGPROC)CDlgFileAssocProc);
	player()->lpWndMain->Repaint(PNT_FORCEFULL, 0);

}

bool CDlgFileAssoc::IsExtAssigned(LPWSTR extName, LPWSTR extDesc) {

	bool result = false;

	HKEY hKey;
	wchar_t buf[100];
	DWORD ln = 100;

	if (RegOpenKeyExW(HKEY_CLASSES_ROOT, extName, 0, 0, &hKey) == ERROR_SUCCESS) {
		if ((RegQueryValueExW(hKey, NULL, NULL, NULL, (BYTE*)&buf, &ln) == ERROR_SUCCESS) && (strCompare(buf, extDesc) == 0)) {
			result = true;
		}
		RegCloseKey(hKey);
	}

	return result;

}

bool CDlgFileAssoc::IsExtAssignedDyn(LPWSTR extName) {

	wchar_t szKeyName[20];
	wchar_t szKeyUpr[20];
	wchar_t szKeyDesc[50];

	wcscpy(szKeyUpr, extName);
	wcsupr(szKeyUpr);

	wsprintf(szKeyName, L".%s", extName);
	wsprintf(szKeyDesc, L"Nitrogen%sFile", szKeyUpr);

	bool result = false;

	HKEY hKey;
	wchar_t buf[100];
	DWORD ln = 100;

	if (RegOpenKeyExW(HKEY_CLASSES_ROOT, szKeyName, 0, 0, &hKey) == ERROR_SUCCESS) {
		if ((RegQueryValueExW(hKey, NULL, NULL, NULL, (BYTE*)&buf, &ln) == ERROR_SUCCESS) && (strCompare(buf, szKeyDesc) == 0)) {
			result = true;
		}
		RegCloseKey(hKey);
	}

	return result;

}

void RegCopyData(HKEY hKeyIn, HKEY hKeyOut) {

	int index = 0;
	DWORD lenName = 200;
	DWORD lenData = 300;
	wchar_t bufName[200];
	BYTE bufData[300];
	DWORD typ;

	while (RegEnumValueW(hKeyIn, index, (LPWSTR)bufName, &lenName, NULL, &typ, (LPBYTE)bufData, &lenData) == ERROR_SUCCESS) {
		
		RegSetValueExW(hKeyOut, bufName, 0, typ, bufData, lenData);

		lenName = 200;
		lenData = 200;
		index++;
	}

	index = 0;
	while (RegEnumKeyExW(hKeyIn, index, (LPWSTR)bufName, &lenName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {

		HKEY hNewKeyIn;
		if (RegOpenKeyExW(hKeyIn, bufName, 0, 0, &hNewKeyIn) == ERROR_SUCCESS) {
			HKEY hNewKeyOut;
			if (RegCreateKeyEx(hKeyOut, bufName, 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hNewKeyOut, NULL) == ERROR_SUCCESS) {
				RegCopyData(hNewKeyIn, hNewKeyOut);
				RegCloseKey(hNewKeyOut);
			}
			RegCloseKey(hNewKeyIn);
		}
		lenName = 200;
		index++;
	
	}


}

void CDlgFileAssoc::ExtAssignDyn(LPWSTR extName, LPWSTR extCmd, int iconIndex) {

	HKEY hKey;
	DWORD len = 100;
	wchar_t buf[100];

	wchar_t szKeyUpr[20];
	wchar_t szKeyName[20];
	wchar_t szKeyDesc[50];
	wchar_t szKeyStr[50];

	wcscpy(szKeyUpr, extName);
	wcsupr(szKeyUpr);

	wsprintf(szKeyName, L".%s", extName);
	wsprintf(szKeyStr, L"%s file", szKeyUpr);
	wsprintf(szKeyDesc, L"Nitrogen%sFile", szKeyUpr);

	RegCreateKeyExW(HKEY_CLASSES_ROOT, szKeyName, 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, NULL);

	if (RegQueryValueExW(hKey, NULL, NULL, NULL, (BYTE*)buf, &len) == ERROR_SUCCESS) {
		if (len > 0) {
			RegSetValueExW(hKey, L"Nitrogen backup", 0, REG_SZ, (BYTE*)buf, len);
		}
	}
	
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)szKeyDesc, wcslen(szKeyDesc)*2+2);

	wchar_t oldSettings[100];
	oldSettings[0] = 0;
	len = 100;

	if (RegQueryValueExW(hKey, L"Nitrogen backup", NULL, NULL, (BYTE*)buf, &len) == ERROR_SUCCESS) {
		wcscpy(oldSettings, buf);
	}

	RegCloseKey(hKey);

	RegCreateKeyExW(HKEY_CLASSES_ROOT, szKeyDesc, 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, NULL);
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)szKeyStr, wcslen(szKeyStr)*2+2);
	RegCloseKey(hKey);

	wchar_t sIco[100];
	wsprintf(sIco, L"%s\\DefaultIcon", szKeyDesc);
	RegCreateKeyExW(HKEY_CLASSES_ROOT, sIco, 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, NULL);
	wchar_t s[MAX_PATH];
	wsprintf(s, L"%sNitrogen.exe,%i", player()->Path, iconIndex);
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)s, wcslen(s)*2+2);
	RegCloseKey(hKey);

	wchar_t sCmd[100];
	wsprintf(sCmd, L"%s\\Shell\\Open\\Command", szKeyDesc);
	RegCreateKeyExW(HKEY_CLASSES_ROOT, sCmd, 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, NULL);
	wsprintf(s, L"\"%sNitrogen.exe\" %s", player()->Path, extCmd);
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)s, wcslen(s)*2+2);
	RegCloseKey(hKey);

	if (oldSettings[0] != 0) {
		HKEY hKeyIn;
		wchar_t PathShellExIn[MAX_PATH];
		wsprintf(PathShellExIn, L"%s\\ShellEx", oldSettings);
		if (RegOpenKeyExW(HKEY_CLASSES_ROOT, PathShellExIn, 0, 0, &hKeyIn) == ERROR_SUCCESS) {
			wchar_t PathShellExOut[MAX_PATH];
			wsprintf(PathShellExOut, L"%s\\ShellEx", szKeyDesc);
			HKEY hKeyOut;
			if (RegCreateKeyExW(HKEY_CLASSES_ROOT, PathShellExOut, 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKeyOut, NULL) == ERROR_SUCCESS) {
				RegCopyData(hKeyIn, hKeyOut);
				RegCloseKey(hKeyOut);
			}
			RegCloseKey(hKeyIn);
		}
	}
}

void CDlgFileAssoc::ExtUnassign(LPWSTR extName) {

	HKEY hKey;
	DWORD len = 100;
	wchar_t buf[100];

	if (RegOpenKeyExW(HKEY_CLASSES_ROOT, extName, 0, 0, &hKey) == ERROR_SUCCESS) {

		if (RegQueryValueExW(hKey, L"Nitrogen backup", NULL, NULL, (BYTE*)buf, &len) == ERROR_SUCCESS) {	

			RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)buf, len);
			RegDeleteValueW(hKey, L"Nitrogen backup");
	
		} else {

			RegDeleteValueW(hKey, NULL);
	
		}
	
		RegCloseKey(hKey);
	}

}

void CDlgFileAssoc::ExtUnassignDyn(LPWSTR extName) {

	HKEY hKey;
	DWORD len = 100;
	wchar_t buf[100];

	wchar_t szKeyName[20];
	wchar_t szKeyUpr[20];
	wchar_t szKeyDesc[50];

	wcscpy(szKeyUpr, extName);
	wcsupr(szKeyUpr);

	wsprintf(szKeyName, L".%s", extName);
	wsprintf(szKeyDesc, L"Nitrogen%sFile", szKeyUpr);

	if (RegOpenKeyExW(HKEY_CLASSES_ROOT, szKeyName, 0, 0, &hKey) == ERROR_SUCCESS) {

		if (RegQueryValueExW(hKey, L"Nitrogen backup", NULL, NULL, (BYTE*)buf, &len) == ERROR_SUCCESS) {	

			RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)buf, len);
			RegDeleteValueW(hKey, L"Nitrogen backup");
	
		} else {

			RegDeleteValueW(hKey, NULL);
	
		}
	
		RegCloseKey(hKey);
	}

	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyDesc);
}

LRESULT CDlgFileAssoc::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		
		case WM_INITDIALOG: {

#ifdef SHELL_AYGSHELL
			// Initializes menubar		
			SHMENUBARINFO mbi;
			memset(&mbi, 0, sizeof(SHMENUBARINFO));
			mbi.cbSize = sizeof(SHMENUBARINFO);
			mbi.hwndParent = hWnd;
			mbi.nToolBarId = MNU_OKCANCEL;
			mbi.hInstRes = player()->hInstance;
			mbi.nBmpId = 0;
			mbi.cBmpImages = 0;
			if (SHCreateMenuBar(&mbi)) {
				hWndMenuBar = mbi.hwndMB;
				SetSoftkeyText(hWndMenuBar, 0, _str(STR_CANCEL));
				SetSoftkeyText(hWndMenuBar, 1, _str(STR_OK));
			} else {
				hWndMenuBar = NULL;
			}
#else
            hWndMenuBar = NULL;
#endif

#ifdef SHELL_AYGSHELL
			// Initializes fullscreen dialog
			SHINITDLGINFO shidi;
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIZEDLGFULLSCREEN|SHIDIF_DONEBUTTON|SHIDIF_WANTSCROLLBAR;
			shidi.hDlg = hWnd;
			SHInitDialog(&shidi);
#endif

			SetWindowText(hWnd, _str(STR_FILEASSOCIATIONS));
			SetDlgItemText(hWnd, LBL_USETOOPEN, _str(STR_USENITROGENTOOPE));
			SetDlgItemText(hWnd, BTN_SELALL, _str(STR_SELECTALL));
			SetDlgItemText(hWnd, BTN_SELNONE, _str(STR_SELECTNONE));
			SetDlgItemText(hWnd, LBL_NOTRUNNING, _str(STR_IFTHEPLAYERISNOT));
			SetDlgItemText(hWnd, LBL_RUNNING, _str(STR_IFTHEPLAYERISALR));

			int rActions[3] = {STR_SETFILEASPLAYLIS, STR_ADDFILETOPLAYLIS, STR_SETFOLDERASPLAYL};
			for (int i = 0; i < 3; i++) {
				SendDlgItemMessage(hWnd, CBB_NOTRUNNING, CB_ADDSTRING, 0, (LPARAM)_str(rActions[i]));
				SendDlgItemMessage(hWnd, CBB_RUNNING, CB_ADDSTRING, 0, (LPARAM)_str(rActions[i]));
			}

			SendDlgItemMessage(hWnd, CBB_NOTRUNNING, CB_SETCURSEL, (WPARAM)player()->lpConfig->cf.FileAssNotRunning, 0);
			SendDlgItemMessage(hWnd, CBB_RUNNING, CB_SETCURSEL, (WPARAM)player()->lpConfig->cf.FileAssRunning, 0);

			ListView_SetExtendedListViewStyle(GetDlgItem(hWnd, LST_EXTENSIONS), LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);
		
			LVCOLUMN nCol;
			memset(&nCol, 0, sizeof(LVCOLUMN));
			nCol.mask = LVCF_TEXT;
			nCol.pszText = NULL;
			ListView_InsertColumn(GetDlgItem(hWnd, LST_EXTENSIONS), 0, &nCol);

			LPWSTR assc[4] = {L"Skin files (*.sk2)", L"MPEG Layer-3 files (*.mp3)", L"Ogg Vorbis files (*.ogg)", L"Wave Audio files (*.wav)"};
			LPWSTR exts[4] = {L"sk2", L"mp3", L"ogg", L"wav"};

			int y = 0;

			for (int i = 0; i < 4; i++) {
				LVITEM nItem;
				memset(&nItem, 0, sizeof(LVITEM));
				nItem.mask = LVIF_TEXT;
				nItem.iSubItem = 0;
				nItem.pszText = assc[i];
				nItem.iItem = y;
				ListView_InsertItem(GetDlgItem(hWnd, LST_EXTENSIONS), &nItem);
				PrevAssoc[y] = IsExtAssignedDyn(exts[i]);
				if (PrevAssoc[y]) {
					ListView_SetCheckState(GetDlgItem(hWnd, LST_EXTENSIONS), y, true);
				}
				y++;
			}
			for (int i = 0; i < MAP_GetDecoderPlugInCount(player()->MAP); i++) {

				MAP_DEC_PLUGIN * mplug = MAP_GetDecoderPlugIn(player()->MAP, i);
				
				for (int x = 0; x < mplug->GetFileExtCount(); x++) {

					TCHAR ExtName[MAX_PATH];
					TCHAR ExtDesc[MAX_PATH];
					mplug->GetFileExt(x, ExtName, ExtDesc);
					LVITEM nItem;
					memset(&nItem, 0, sizeof(LVITEM));
					nItem.mask = LVIF_TEXT;
					nItem.iSubItem = 0;
					nItem.pszText = ExtDesc;
					nItem.iItem = y;
					
					ListView_InsertItem(GetDlgItem(hWnd, LST_EXTENSIONS), &nItem);	
					PrevAssoc[y] = IsExtAssignedDyn(ExtName);
					if (PrevAssoc[y]) {
						ListView_SetCheckState(GetDlgItem(hWnd, LST_EXTENSIONS), y, true);
					}

					y++;
				}
			}

			RECT rc;
			GetClientRect(GetDlgItem(hWnd, LST_EXTENSIONS), &rc);
			ListView_SetColumnWidth(GetDlgItem(hWnd, LST_EXTENSIONS), 0, rc.right-rc.left);

			break;

		}

 		case WM_COMMAND: {

			int wmId = LOWORD(wParam); 
			if ((wmId == IDM_OK) || (wmId == IDOK)) {

				player()->lpConfig->cf.FileAssNotRunning = SendDlgItemMessage(hWnd, CBB_NOTRUNNING, CB_GETCURSEL, 0, 0);
				player()->lpConfig->cf.FileAssRunning = SendDlgItemMessage(hWnd, CBB_RUNNING, CB_GETCURSEL, 0, 0);
				
				LPWSTR exts[4] = {L"sk2", L"mp3", L"ogg", L"wav"};
				for (int i = 0; i < 4; i++) {
					
					bool NewAssoc = ListView_GetCheckState(GetDlgItem(hWnd, LST_EXTENSIONS), i);

					if (NewAssoc && !PrevAssoc[i]) {
						ExtAssignDyn(exts[i], L"\"%1\"", 102+(int)(i == 0));
					}

					if (!NewAssoc && PrevAssoc[i]) {
						ExtUnassignDyn(exts[i]);
					}

				}

				int y = 4;

				for (int i = 0; i < MAP_GetDecoderPlugInCount(player()->MAP); i++) {
		
					MAP_DEC_PLUGIN * mplug = MAP_GetDecoderPlugIn(player()->MAP, i);
				
					for (int x = 0; x < mplug->GetFileExtCount(); x++) {

						bool NewAssoc = ListView_GetCheckState(GetDlgItem(hWnd, LST_EXTENSIONS), y);
	
						TCHAR ExtName[MAX_PATH];
						TCHAR ExtDesc[MAX_PATH];
						mplug->GetFileExt(x, ExtName, ExtDesc);
						
						if (NewAssoc && !PrevAssoc[y]) {
							ExtAssignDyn(ExtName, L"\"%1\"", 102);
						}

						if (!NewAssoc && PrevAssoc[y]) {
							ExtUnassignDyn(ExtName);
						}

						y++;
					}
				}

				EndDialog(hWnd, 0);

			} else if (wmId == BTN_SELALL) {
		
				HWND ls = GetDlgItem(hWnd, LST_EXTENSIONS);
				for (int i = 0; i < ListView_GetItemCount(ls); i++) {
					ListView_SetCheckState(ls, i, true);
				}

			} else if (wmId == BTN_SELNONE) {
		
				HWND ls = GetDlgItem(hWnd, LST_EXTENSIONS);
				for (int i = 0; i < ListView_GetItemCount(ls); i++) {
					ListView_SetCheckState(ls, i, false);
				}

			} else if (wmId == IDM_CANCEL) {
			
				EndDialog(hWnd, 0);
			
			}

			break;
		}

		case WM_DESTROY:
			CommandBar_Destroy(hWndMenuBar);
			break;

		case WM_ACTIVATE:
			
			if ((wParam == WA_INACTIVE) && ((HWND)lParam == player()->lpWndMain->hWnd)) {
				EndDialog(hWnd, 0);
			} else {
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			break;

		default:
			DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK CDlgFileAssocProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	if (uMsg == WM_INITDIALOG) {
		player()->lpDlgFileAssoc->hWnd = hWnd;
	}
	return player()->lpDlgFileAssoc->DlgProc(uMsg, wParam, lParam);

}
