#include "stdafx.h"
#include "main.h"

void CDlgSettings::Execute() {

	DialogBox(player()->hInstance, MAKEINTRESOURCE(DLG_SETTINGS), player()->lpWndMain->hWnd, (DLGPROC)CDlgSettingsProc);
	player()->lpWndMain->Repaint(PNT_FORCEFULL, 0);

}

void CDlgSettings::UpdateUpdown(int updCode, int edtCode) {

	BOOL conv;
	int x = GetDlgItemInt(hWnd, edtCode, &conv, true);
	if (conv) {
		SendDlgItemMessage(hWnd, updCode, UDM_SETPOS, 0, x);
	}

}

LRESULT CDlgSettings::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

			SetWindowText(hWnd, _str(STR_GENERALSETTINGS));
			SetDlgItemText(hWnd, LBL_THREADPRIORITY, _str(STR_THREADPRIORITY));
			SetDlgItemText(hWnd, LBL_SLEEPACTION, _str(STR_SLEEPACTION));
			SetDlgItemText(hWnd, LBL_PBLENGTH, _str(STR_PREBUFFERLENGTH));
			SetDlgItemText(hWnd, LBL_PREBUFFER, _str(STR_OUTPUTPREBUFFER));
			SetDlgItemText(hWnd, LBL_LSOFT, _str(STR_LEFTSOFTKEYACTIO));

			// Adds items to "Thread priority" combobox
			HWND hCbbPriority = GetDlgItem(hWnd, CBB_THPRIORITY);
			SendMessage(hCbbPriority, CB_ADDSTRING, 0, (LPARAM)_str(STR_LOW));
			SendMessage(hCbbPriority, CB_ADDSTRING, 0, (LPARAM)_str(STR_NORMAL));

			if (player()->lpConfig->cf.LowThreadPriority) {
				SendMessage(hCbbPriority, CB_SETCURSEL, 0, 0);
			} else {
				SendMessage(hCbbPriority, CB_SETCURSEL, 1, 0);
			}

			// Adds items to "Sleep action" combobox
			HWND hCbbSleep = GetDlgItem(hWnd, CBB_SLEEP);
			SendMessage(hCbbSleep, CB_ADDSTRING, 0, (LPARAM)_str(STR_STOPMUSIC));
			SendMessage(hCbbSleep, CB_ADDSTRING, 0, (LPARAM)_str(STR_CLOSEAPPLICATION));
			SendMessage(hCbbSleep, CB_ADDSTRING, 0, (LPARAM)_str(STR_STANDBY));

			SendMessage(hCbbSleep, CB_SETCURSEL, player()->lpConfig->cf.SleepAction, 0);

			// Adds items to "Left softkey action" combobox
			HWND hCbbLSoftkey = GetDlgItem(hWnd, CBB_LSOFTKEY);
			SendMessage(hCbbLSoftkey, CB_ADDSTRING, 0, (LPARAM)_str(STR_CLOSEAPPLICATION));
			SendMessage(hCbbLSoftkey, CB_ADDSTRING, 0, (LPARAM)_str(STR_MINIMIZEPLAYER));
			SendMessage(hCbbLSoftkey, CB_ADDSTRING, 0, (LPARAM)_str(STR_BLANKSCREEN));
			SendMessage(hCbbLSoftkey, CB_ADDSTRING, 0, (LPARAM)_str(STR_MEDIABROWSER));

			SendMessage(hCbbLSoftkey, CB_SETCURSEL, player()->lpConfig->cf.LeftSoftkeyAction, 0);

			UDACCEL acc[1];
			acc[0].nInc = 100;
			acc[0].nSec = 0;

			HWND hUpdPBLength = GetDlgItem(hWnd, UPD_PBLENGTH);
			SendMessage(hUpdPBLength, UDM_SETRANGE, 0, MAKELONG(5000, 1000));
			SendMessage(hUpdPBLength, UDM_SETACCEL, 1, (LPARAM)&acc);
			SendMessage(hUpdPBLength, UDM_SETPOS, 0, player()->lpConfig->cf.OutputBufferLen);
			SetDlgItemInt(hWnd, EDT_PBLENGTH, player()->lpConfig->cf.OutputBufferLen, true);
			
			acc[0].nInc = 5;
			acc[0].nSec = 0;

			HWND hUpdOPrebuff = GetDlgItem(hWnd, UPD_OPREBUFF);
			SendMessage(hUpdOPrebuff, UDM_SETRANGE, 0, MAKELONG(100, 0));
			SendMessage(hUpdOPrebuff, UDM_SETACCEL, 1, (LPARAM)&acc);
			SendMessage(hUpdOPrebuff, UDM_SETPOS, 0, player()->lpConfig->cf.OutputPrebuffer);
			SetDlgItemInt(hWnd, EDT_OPREBUFF, player()->lpConfig->cf.OutputPrebuffer, true);

			ListView_SetExtendedListViewStyle(GetDlgItem(hWnd, LST_OPTIONS), LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);
		
			LVCOLUMN nCol;
			memset(&nCol, 0, sizeof(LVCOLUMN));
			nCol.mask = LVCF_TEXT;
			nCol.pszText = NULL;
			ListView_InsertColumn(GetDlgItem(hWnd, LST_OPTIONS), 0, &nCol);

			bool chk[7] = {player()->lpConfig->cf.ForceCleartype, player()->lpConfig->cf.LockHardKeys, player()->lpConfig->cf.ChangeSongButtons, player()->lpConfig->cf.Id3Tags, player()->lpConfig->cf.RememberPos, player()->lpConfig->cf.WriteSongRegistry, player()->lpConfig->cf.useSystemVolume};
			int str[7]  = {STR_FORCECLEARTYPERE, STR_LOCKHARDWAREKEYS, STR_CHANGEPLAYINGSON, STR_READIDTAGSWHENAV, STR_REMEMBERLASTSONG, STR_WRITECURRENTSONG, STR_USESYSTEMVOLUME};

			for (int i = 0; i < 7; i++) {
				LVITEM nItem;
				memset(&nItem, 0, sizeof(LVITEM));
				nItem.mask = LVIF_TEXT;
				nItem.iSubItem = 0;
				nItem.pszText = _str(str[i]);
				nItem.iItem = i;
				ListView_InsertItem(GetDlgItem(hWnd, LST_OPTIONS), &nItem);
				if (chk[i]) {
					ListView_SetCheckState(GetDlgItem(hWnd, LST_OPTIONS), i, true);
				}
			}

			RECT rc;
			GetClientRect(GetDlgItem(hWnd, LST_OPTIONS), &rc);
			ListView_SetColumnWidth(GetDlgItem(hWnd, LST_OPTIONS), 0, rc.right-rc.left);

			break;

		}

		case WM_VSCROLL: {

			if ((HWND)lParam == GetDlgItem(hWnd, UPD_PBLENGTH)) {
				SetDlgItemInt(hWnd, EDT_PBLENGTH, HIWORD(wParam), true);

			} else if ((HWND)lParam == GetDlgItem(hWnd, UPD_OPREBUFF)) {
				SetDlgItemInt(hWnd, EDT_OPREBUFF, HIWORD(wParam), true);

			} else {
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}

			break;

		}

 		case WM_COMMAND: {

			int wmId = LOWORD(wParam); 
			if ((wmId == IDM_OK) || (wmId == IDOK)) {
			
				int cbSleepSel = SendDlgItemMessage(hWnd, CBB_SLEEP, CB_GETCURSEL, 0, 0);			
				player()->lpConfig->cf.SleepAction = (SLEEPACTION) cbSleepSel;

				// Saves the current player status to be restored later
				MAP_STATUS prevStatus = MAP_GetStatus(player()->MAP);
				int seekTime = 0;
				if (MAP_GetStatus(player()->MAP) != MAP_STATUS_STOP) {
					
					seekTime = MAP_GetCurrent(player()->MAP);
					MAP_Stop(player()->MAP);

				}

				MAP_Close(player()->MAP);
				
				MAP_OPTIONS mop;
				MAP_GetOptions(player()->MAP, &mop);
				
				BOOL conv;
				int res = GetDlgItemInt(hWnd, EDT_PBLENGTH, &conv, true);
				if (conv) {
					if (res < 1000) { res = 1000; }
					if (res > 5000) { res = 5000; }
					player()->lpConfig->cf.OutputBufferLen = res;				
					mop.nOutputBufferLen = res;
				}

				res = GetDlgItemInt(hWnd, EDT_OPREBUFF, &conv, true);
				if (conv) {
					if (res < 0) { res = 0; }
					if (res > 100) { res = 100; }
					player()->lpConfig->cf.OutputPrebuffer = res;
					mop.nOutputPrebuffer = res;
				}

				int cbPrioritySel = SendDlgItemMessage(hWnd, CBB_THPRIORITY, CB_GETCURSEL, 0, 0);
				if (cbPrioritySel == 0) {
					player()->lpConfig->cf.LowThreadPriority = true;
					mop.nThreadPriority = THREAD_PRIORITY_BELOW_NORMAL;
				} else {
					player()->lpConfig->cf.LowThreadPriority = false;
					mop.nThreadPriority = THREAD_PRIORITY_NORMAL;
				}

				MAP_SetOptions(player()->MAP, &mop);

				if ((prevStatus == MAP_STATUS_PAUSE) || (prevStatus == MAP_STATUS_PLAY)) {
					
					MAP_Open(player()->MAP, player()->lpPlaylist->Data[player()->PlayingIndex].FileName);
					MAP_Seek(player()->MAP, seekTime);
					if (prevStatus == MAP_STATUS_PLAY) {
						MAP_Play(player()->MAP);
					}
				}

				player()->lpConfig->cf.ForceCleartype = (ListView_GetCheckState(GetDlgItem(hWnd, LST_OPTIONS), 0) == TRUE);
				if (ListView_GetCheckState(GetDlgItem(hWnd, LST_OPTIONS), 1) && !player()->KeyLockSupport) {
					MessageBox(hWnd, _str(STR_YOURDEVICEHASNOK), _str(STR_ERROR), MB_ICONERROR|MB_OK);
				} else {
					player()->lpConfig->cf.LockHardKeys = (ListView_GetCheckState(GetDlgItem(hWnd, LST_OPTIONS), 1) == TRUE);
				}
				player()->lpConfig->cf.ChangeSongButtons = (ListView_GetCheckState(GetDlgItem(hWnd, LST_OPTIONS), 2) == TRUE);
				player()->lpConfig->cf.Id3Tags = (ListView_GetCheckState(GetDlgItem(hWnd, LST_OPTIONS), 3) == TRUE);
				player()->lpConfig->cf.RememberPos = (ListView_GetCheckState(GetDlgItem(hWnd, LST_OPTIONS), 4) == TRUE);
				player()->lpConfig->cf.WriteSongRegistry = (ListView_GetCheckState(GetDlgItem(hWnd, LST_OPTIONS), 5) == TRUE);
				player()->lpConfig->cf.useSystemVolume = (ListView_GetCheckState(GetDlgItem(hWnd, LST_OPTIONS), 6) == TRUE);
				player()->lpConfig->cf.LeftSoftkeyAction = (LSOFTKEYACTION)SendDlgItemMessage(hWnd, CBB_LSOFTKEY, CB_GETCURSEL, 0, 0);
				player()->lpWndMain->SetLSoftkeyText();
				HKEY hKey;
			
				if (player()->lpConfig->cf.WriteSongRegistry && RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"System\\State\\Nitrogen", 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, NULL) == ERROR_SUCCESS) {
	
					DWORD buf = 1;
					RegSetValueExW(hKey, L"Running", 0, REG_DWORD, (BYTE*)&buf, 4);
					RegCloseKey(hKey);

				}

				EndDialog(hWnd, 0);
			
			} else if (wmId == IDM_CANCEL) {
			
				EndDialog(hWnd, 0);
			
			}
			break;
		}

		case WM_DESTROY:
			EndDialog(hWnd, 0);
			CommandBar_Destroy(hWndMenuBar);
			break;

		case WM_NOTIFY: {

			LPNMHDR not = (LPNMHDR)lParam;
			if (not->code == UDN_DELTAPOS) {
	
				if (not->hwndFrom == GetDlgItem(hWnd, UPD_PBLENGTH)) {
					UpdateUpdown(UPD_PBLENGTH, EDT_PBLENGTH);
				}

				if (not->hwndFrom == GetDlgItem(hWnd, UPD_OPREBUFF)) {
					UpdateUpdown(UPD_OPREBUFF, EDT_OPREBUFF);
				}

			} else {
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}

			break;
		}

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

LRESULT CALLBACK CDlgSettingsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	if (uMsg == WM_INITDIALOG) {
		player()->lpDlgSettings->hWnd = hWnd;
	}
	return player()->lpDlgSettings->DlgProc(uMsg, wParam, lParam);

}
