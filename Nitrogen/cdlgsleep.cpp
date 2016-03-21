#include "stdafx.h"
#include "main.h"

CDlgSleep::CDlgSleep() {
	
	SleepActive = false;
	SleepMins = 0;
}

void CDlgSleep::Execute() {
	
	DialogBox(player()->hInstance, MAKEINTRESOURCE(DLG_SLEEP), player()->lpWndMain->hWnd, (DLGPROC)CDlgSleepProc);
	player()->lpWndMain->Repaint(PNT_FORCEFULL, 0);

}

void CDlgSleep::UpdateText() {

	wchar_t ws[15];
	wsprintf(ws, L"%02i %s", SleepMins, _str(STR_MINUTES));
	SetDlgItemText(hWnd, LBL_TIME, ws);

}

LRESULT CDlgSleep::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		
		case WM_INITDIALOG: {
		
			KillTimer(player()->lpWndMain->hWnd, TMR_SLEEP);

			SetDlgItemText(hWnd, CHK_ACTIVE, _str(STR_ACTIVE));
			SetDlgItemText(hWnd, BTN_OK, _str(STR_OK));
			SetWindowText(hWnd, _str(STR_SLEEPMODE));

			SendDlgItemMessage(hWnd, SLD_TIME, TBM_SETRANGE, (WPARAM)false, (LPARAM)MAKELONG(1, 120));
			
			HDC hDC = CreateCompatibleDC(NULL);
			TEXTMETRIC tMet;
			GetTextMetrics(hDC, &tMet);
			HFONT hf = CreateFont(1.5*tMet.tmHeight, FW_BOLD, false);
			SendDlgItemMessage(hWnd, LBL_TIME, WM_SETFONT, (WPARAM)hf, TRUE);
			DeleteDC(hDC);

			if (SleepActive) {
				CheckDlgButton(hWnd, CHK_ACTIVE, BST_CHECKED);
			} else {
				SleepMins = 30;
			}

			UpdateText();

			SendDlgItemMessage(hWnd, SLD_TIME, TBM_SETPOS, (WPARAM)true, (LPARAM)SleepMins);
			break;
		}

		case WM_HSCROLL: {
	
			HWND hSldTime = GetDlgItem(hWnd, SLD_TIME);
			if ((HWND)lParam == hSldTime) {
				SleepMins = SendMessage(hSldTime, TBM_GETPOS, 0, 0);
				UpdateText();
			}
			break;

		}

		case WM_COMMAND: {

			int wmId = LOWORD(wParam); 
			if (wmId == BTN_OK) {

				HWND hSldTime = GetDlgItem(hWnd, SLD_TIME);
				if (IsDlgButtonChecked(hWnd, CHK_ACTIVE) == BST_CHECKED) {
					
					SleepActive = true;
					player()->lpWndMain->MenuMain.Data[3].Checked = true;
					SleepMins = SendMessage(hSldTime, TBM_GETPOS, 0, 0);
					iSetCmdToggle(ICMD_SLEEPMODE, true);
				
				} else {
				
					SleepActive = false;
					player()->lpWndMain->MenuMain.Data[3].Checked = false;
					iSetCmdToggle(ICMD_SLEEPMODE, false);
				
				}
				EndDialog(hWnd, 1);
			}
			break;
		}

		case WM_ACTIVATE: {

			if ((wParam == WA_INACTIVE) & ((HWND)lParam == player()->lpWndMain->hWnd)) {
			
				if (SleepActive) {
					SetTimer(player()->lpWndMain->hWnd, TMR_SLEEP, SLEEP_INTERVAL, NULL);
				}
				EndDialog(hWnd, 0);

			} else {
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			break;

		}

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

	}
	return 0;
}

LRESULT CALLBACK CDlgSleepProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	if (uMsg == WM_INITDIALOG) {
		player()->lpDlgSleep->hWnd = hWnd;
	}
	return player()->lpDlgSleep->DlgProc(uMsg, wParam, lParam);

}
