#include "stdafx.h"
#include "main.h"

void CDlgLists::Execute() {

	DialogBox(player()->hInstance, MAKEINTRESOURCE(DLG_LISTS), player()->lpWndMain->hWnd, (DLGPROC)CDlgListsProc);
	player()->lpWndMain->Repaint(PNT_FORCEFULL, 0);

}

void CDlgLists::UpdateUpdown(int updCode, int edtCode) {

	BOOL conv;
	int x = GetDlgItemInt(hWnd, edtCode, &conv, true);
	if (conv) {
		SendDlgItemMessage(hWnd, updCode, UDM_SETPOS, 0, x);
	}

}

LRESULT CDlgLists::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

			SetWindowText(hWnd, _str(STR_LISTSANDBROWSERS));
			SetDlgItemText(hWnd, CHK_ENABLEHAPTICS, _str(STR_ENABLEKINETICSCR));
			SetDlgItemText(hWnd, LBL_KINETIC, _str(STR_KINETICSCROLLING));
			SetDlgItemText(hWnd, LBL_REFINTERVAL, _str(STR_REFRESHINTERVAL));
			SetDlgItemText(hWnd, LBL_INITSPEED, _str(STR_INITIALSPEED));
			SetDlgItemText(hWnd, LBL_DEACCEL, _str(STR_DEACCELERATION));
			SetDlgItemText(hWnd, LBL_MINGESTURE, _str(STR_MINIMALGESTURE));
			SetDlgItemText(hWnd, CHK_HAPDRAFT, _str(STR_LOWERFONTQUALITY));

			UDACCEL acc[1];
			acc[0].nInc = 1;
			acc[0].nSec = 0;
			int upds[4] = {UPD_REFINTERVAL, UPD_INITSPEED, UPD_DEACCEL, UPD_MINGESTURE};
			for (int i = 0; i < 4; i++) {
				SendDlgItemMessage(hWnd, upds[i], UDM_SETACCEL, 1, (LPARAM)&acc);
			}

			SetDlgItemInt(hWnd, EDT_REFINTERVAL, player()->lpConfig->cf.hapRefreshInterval, true);
			SetDlgItemInt(hWnd, EDT_INITSPEED, player()->lpConfig->cf.hapInitialSpeed, true);
			SetDlgItemInt(hWnd, EDT_DEACCEL, player()->lpConfig->cf.hapDeacceleration, true);
			SetDlgItemInt(hWnd, EDT_MINGESTURE, player()->lpConfig->cf.hapMinGesture, true);
			
			if (player()->lpConfig->cf.hapEnabled) {
				CheckDlgButton(hWnd, CHK_ENABLEHAPTICS, BST_CHECKED);
			}
			
			if (player()->lpConfig->cf.hapDraftQuality) {
				CheckDlgButton(hWnd, CHK_HAPDRAFT, BST_CHECKED);
			}
			
			SendDlgItemMessage(hWnd, UPD_REFINTERVAL, UDM_SETRANGE, 0, (LPARAM)MAKELONG(500, 1));
			SendDlgItemMessage(hWnd, UPD_INITSPEED, UDM_SETRANGE, 0, (LPARAM)MAKELONG(200, 1));
			SendDlgItemMessage(hWnd, UPD_MINGESTURE, UDM_SETRANGE, 0, (LPARAM)MAKELONG(200, 1));
			SendDlgItemMessage(hWnd, UPD_DEACCEL, UDM_SETRANGE, 0, (LPARAM)MAKELONG(200, 1));

			break;

		}

		case WM_VSCROLL: {

			if ((HWND)lParam == GetDlgItem(hWnd, UPD_REFINTERVAL)) {
				SetDlgItemInt(hWnd, EDT_REFINTERVAL, HIWORD(wParam), true);

			} else if ((HWND)lParam == GetDlgItem(hWnd, UPD_INITSPEED)) {
				SetDlgItemInt(hWnd, EDT_INITSPEED, HIWORD(wParam), true);

			} else if ((HWND)lParam == GetDlgItem(hWnd, UPD_DEACCEL)) {
				SetDlgItemInt(hWnd, EDT_DEACCEL, HIWORD(wParam), true);

			} else if ((HWND)lParam == GetDlgItem(hWnd, UPD_MINGESTURE)) {
				SetDlgItemInt(hWnd, EDT_MINGESTURE, HIWORD(wParam), true);

			} else {
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}

			break;

		}

 		case WM_COMMAND: {

			int wmId = LOWORD(wParam); 
			if ((wmId == IDM_OK) || (wmId == IDOK)) {			

				player()->lpConfig->cf.hapEnabled = (IsDlgButtonChecked(hWnd, CHK_ENABLEHAPTICS) == BST_CHECKED);
				player()->lpConfig->cf.hapDraftQuality = (IsDlgButtonChecked(hWnd, CHK_HAPDRAFT) == BST_CHECKED);
				BOOL b;

				int res = GetDlgItemInt(hWnd, EDT_REFINTERVAL, &b, true);
				if (b) {
					if (res < 1) { res = 1; }
					if (res > 500) { res = 500; }
					player()->lpConfig->cf.hapRefreshInterval = res;
				}

				res = GetDlgItemInt(hWnd, EDT_INITSPEED, &b, true);
				if (b) {
					if (res < 1) { res = 1; }
					if (res > 200) { res = 200; }
					player()->lpConfig->cf.hapInitialSpeed = res;
				}

				res = GetDlgItemInt(hWnd, EDT_MINGESTURE, &b, true);
				if (b) {
					if (res < 1) { res = 1; }
					if (res > 200) { res = 200; }
					player()->lpConfig->cf.hapMinGesture = res;
				}

				res = GetDlgItemInt(hWnd, EDT_DEACCEL, &b, true);
				if (b) {
					if (res < 1) { res = 1; }
					if (res > 200) { res = 200; }
					player()->lpConfig->cf.hapDeacceleration = res;
				}

				EndDialog(hWnd, 0);
			
			} else if (wmId == IDM_CANCEL) {
			
				EndDialog(hWnd, 0);
			
			}
			break;
		}

		case WM_DESTROY:
			CommandBar_Destroy(hWndMenuBar);
			break;

		case WM_NOTIFY: {

			LPNMHDR not = (LPNMHDR)lParam;
			if (not->code == UDN_DELTAPOS) {
	
				int upds[4] = {UPD_REFINTERVAL, UPD_INITSPEED, UPD_DEACCEL, UPD_MINGESTURE};
				int edts[4] = {EDT_REFINTERVAL, EDT_INITSPEED, EDT_DEACCEL, EDT_MINGESTURE};

				for (int i = 0; i < 4; i++) {
					if (not->hwndFrom == GetDlgItem(hWnd, upds[i])) {
						UpdateUpdown(upds[i], edts[i]);
					}
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

LRESULT CALLBACK CDlgListsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	if (uMsg == WM_INITDIALOG) {
		player()->lpDlgLists->hWnd = hWnd;
	}
	return player()->lpDlgLists->DlgProc(uMsg, wParam, lParam);

}
