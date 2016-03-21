#include "stdafx.h"
#include "main.h"

LPWSTR eqFreq[11] = {L"Pre", L"60", L"170", L"310", L"600", L"1", L"3", L"6", L"12", L"14", L"16"};
LPWSTR eqHz[11] = {L"amp", L"Hz", L"Hz", L"Hz", L"Hz", L"kHz", L"kHz", L"kHz", L"kHz", L"kHz", L"kHz"};


void CWndEqualizer::CreateMenuBar(LPWSTR lText, LPWSTR rText) {
	return;
#ifdef SHELL_AYGSHELL

#if _WIN32_WCE < 0x500
	if (hWndMenuBar) {
		CommandBar_Destroy(hWndMenuBar);
		hWndMenuBar = NULL;
	}
#endif

	if (!hWndMenuBar) {
		// Creates menu bar
	    SHMENUBARINFO mbi;
	    memset(&mbi, 0, sizeof(SHMENUBARINFO));
	    mbi.cbSize = sizeof(SHMENUBARINFO);
	    mbi.hwndParent = hWnd;
	    mbi.nToolBarId = MNU_EMPTY;
	    mbi.hInstRes = player()->hInstance;
	    if (!SHCreateMenuBar(&mbi)) {
	        hWndMenuBar = NULL;
	    } else {
	        hWndMenuBar = mbi.hwndMB;
	    }
	}
	SetSoftkeyText(hWndMenuBar, IDM_LSOFT, lText);
	SetSoftkeyText(hWndMenuBar, IDM_RSOFT, rText);
#endif
}

ATOM CWndEqualizer::InitClass() {
	WNDCLASS wc;
	wc.style  = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = CWndEqualizerProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = player()->hInstance;
	wc.hIcon = LoadIcon(player()->hInstance, MAKEINTRESOURCE(ICO_MAIN));
	wc.hCursor = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"CNtgWndEqualizer";
	return RegisterClass(&wc);
}

bool CWndEqualizer::Show() {
	HWND hWnd = CreateWindow(L"CNtgWndEqualizer", _str(STR_EQUALIZER), WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, player()->hInstance, NULL);
	if (!hWnd) {
		return false;
    }
#ifdef SHELL_AYGSHELL
	SHDoneButton(hWnd, SHDB_SHOW);
#endif
	if (hWndMenuBar) {
        RECT rc;
        RECT rcMenuBar;
        GetWindowRect(hWnd, &rc);
        GetWindowRect(hWndMenuBar, &rcMenuBar);
        rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }
	ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
	return true;
}

void CWndEqualizer::OnCreate() {

	hWndMenuBar = NULL;
	CreateMenuBar(_str(STR_PRESETS), _str(STR_OK));
	
	SetWbrItem(&MenuClose, _str(STR_BACKTOPREVIOUSME), ICO_CLOSEMENU, false, false, false);
	SetWbrItem(&MenuPresetAdd, _str(STR_ADDASPRESET), ICO_ADDPRESET, false, false, false);
	SetWbrItem(&MenuPresetDel, _str(STR_REMOVEPRESET), ICO_REMPRESET, false, true, false);
	SetWbrItem(&MenuPresetClr, _str(STR_RESETEQUALIZER), ICO_RESETEQ, false, true, false);

	hBoxRect.left = 2;
	hBoxRect.right = get_devWid();
	
	wchar_t fName[MAX_PATH];
	wsprintf(fName, L"%spreset.eql", player()->FilesPath);
	HANDLE fl = CreateFile(fName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (fl != INVALID_HANDLE_VALUE) {
	
		DWORD n = 0;
		ReadFile(fl, &presetCount, 2, &n, NULL); // Number of presets
		preset = new EQPRESET[presetCount];
		for (int i = 0; i < presetCount; i++) {
			memset(&preset[i], 0, sizeof(EQPRESET));
			int slen = 0;
			ReadFile(fl, &slen, 2, &n, NULL);
			ReadFile(fl, preset[i].name, slen*sizeof(wchar_t), &n, NULL);
			preset[i].name[slen] = 0;
			ReadFile(fl, &preset[i].eq.preamp, 1, &n, NULL);
			for (int x = 0; x < 10; x++) {
				ReadFile(fl, &preset[i].eq.data[x], 1, &n, NULL);
			}
		}
		CloseHandle(fl);
	
	} else {

		presetCount = 0;
		preset = NULL;

	}

	ShowingMenu = false;
	lpMenu = new CMenu(hWnd, CWndEqualizerMenuProc);

	selBand = -1;
	MAP_GetEqualizer(player()->MAP, &eq);
}

void CWndEqualizer::OnMouseDown(int px, int py) {
	
	eqClick = -1;
	if (py <= hBoxRect.top) {
		SendMessage(hWnd, WM_CLOSE,0,0);
	} else {
		if ((py > hBoxRect.top) && (py < hBoxRect.bottom)) {
			eqClick = (int)floor((double)(px-hBoxRect.left)*12/(hBoxRect.right-hBoxRect.left)+0.5)-1;
			selBand = eqClick;
			if ((eqClick < 0) || (eqClick > 10)) {
				selBand = -1;
				eqClick = -1;
				InvalidateRect(hWnd, NULL, false);
			} else {
				OnMouseMove(px, py);
			}

		} else {
			if (iPointInRect(px, py, getRect(hBoxRect.left+5, hBoxRect.bottom+3, get_devWid()/2, hBoxRect.bottom+getIcons()->IconSize))) {
				eq.fEnable = !eq.fEnable;
				InvalidateRect(hWnd, NULL, false);
			} else {
				if (iPointInRect(px, py, getRect(get_devWid()/2, hBoxRect.bottom+3, get_devWid()-5, hBoxRect.bottom+getIcons()->IconSize))) {
					StartMenu();
				}
			}
		}
	}
}

void CWndEqualizer::OnMouseMove(int px, int py) {

	if (eqClick >= 0) {
		int ytOff = fHeight;
		int ybOff = fHeight*2;
		int dt = (py-hBoxRect.top-ytOff)*63/(hBoxRect.bottom-hBoxRect.top-ytOff-ybOff);
		if (dt > 63) { dt = 63; }
		if (dt < 0) { dt = 0; }
		if (eqClick == 0) {
			eq.preamp = dt;
		} else {
			eq.data[eqClick-1] = dt;
		}
		InvalidateRect(hWnd, NULL, false);
	}

}

void CWndEqualizer::OnMouseUp(int px, int py) {

	eqClick = -1;
	MAP_SetEqualizer(player()->MAP, &eq);

}

CWndEqualizer::CWndEqualizer() {
	
	fHeight = 0;
	
	Menu.Count = 0;
	lpMenu = NULL;
	
	preset = NULL;
	presetCount = 0;
	eqClick = 0;
	selBand = 0;
	ShowingMenu = false;
	
}

void CWndEqualizer::OnPaint(HDC hDC) {

	if (fHeight == 0) {
		TEXTMETRIC sz = {0};
		GetTextMetrics(hDC, &sz);
		fHeight = sz.tmHeight;
	}

	//int hei = min(get_devHei()-30, (int)(get_devWid()*0.7));
	
	if ((hBoxRect.top == 0) & (hBoxRect.bottom == 0)) {
		//hBoxRect.top = (int)(fHeight*1.5+4);
		hBoxRect.top = (int)(getIcons()->IconSize+10+4);
		hBoxRect.bottom = get_devHei()-getIcons()->IconSize-10;
		if (hWndMenuBar) {
			RECT rcMenuBar;
			GetWindowRect(hWndMenuBar, &rcMenuBar);
			hBoxRect.bottom -=(rcMenuBar.bottom - rcMenuBar.top);
		}
	}

	HBITMAP hBufBmp = CreateBitmap(get_devWid(), get_devHei(), 1, 16, NULL);
	HDC hBufDC = CreateCompatibleDC(NULL);
	HBITMAP hBufBmpOld = (HBITMAP)SelectObject(hBufDC, hBufBmp);

	LPRECT fsRect = getRect(0, 0, get_devWid(), get_devHei());
	HBRUSH fsBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hBufDC, fsRect, fsBrush);
	DeleteObject(fsBrush);
	delete fsRect;

	// Рисуем иконки управления
	if ((getIcons()->Count > ICO_CLOSEMENU) && (getIcon(ICO_CLOSEMENU)->Exists)) {
		TransparentBlt(hBufDC, 5, 5, getIcons()->IconSize, getIcons()->IconSize, getIcon(ICO_CLOSEMENU)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
	}

	// Название окна
	DrawText(hBufDC, _str(STR_GRAPHICEQUALIZER), wcslen(_str(STR_GRAPHICEQUALIZER)), getRect(5, 5, get_devWid()-5, getIcons()->IconSize), DT_CENTER|DT_VCENTER);


	// Элементы управления в нижней строке
	iDrawCheckbox(hBufDC, hBoxRect.left+5, hBoxRect.bottom+3, get_devWid()/2, getIcons()->IconSize, _str(STR_ENABLEEQUALIZER), (eq.fEnable == TRUE));
	DrawText(hBufDC, _str(STR_PRESETS), wcslen(_str(STR_PRESETS)), getRect(get_devWid()/2, hBoxRect.bottom+3, get_devWid()-5, hBoxRect.bottom+getIcons()->IconSize), DT_RIGHT|DT_VCENTER);


	HBRUSH boxBrush = CreateSolidBrush(RGB(230, 230, 230));
	HPEN boxPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	SelectObject(hBufDC, boxBrush); 
	SelectObject(hBufDC, boxPen); 

	Rectangle(hBufDC, hBoxRect.left, hBoxRect.top, hBoxRect.right, hBoxRect.bottom);

	DeleteObject(boxPen);

	SetBkColor(hBufDC, RGB(230, 230, 230));

	int ytOff = fHeight;
	int ybOff = fHeight*2;

	HPEN selPen = CreatePen(PS_DASH, 1, RGB(100, 100, 100));

	if (selBand >= 0) {
		
		SelectObject(hBufDC, selPen);
		int bandCenter = hBoxRect.left+(selBand+1)*(hBoxRect.right-hBoxRect.left)/12;
		int bandWid = (hBoxRect.right-hBoxRect.left)/22-1;
		Rectangle(hBufDC, bandCenter-bandWid, hBoxRect.top+ytOff-2, bandCenter+bandWid, hBoxRect.bottom-ybOff+2);

	}

	HPEN tracePen = CreatePen(PS_DASH, 1, RGB(160, 160, 160));
	SelectObject(hBufDC, tracePen);
	DeleteObject(selPen);

	int scWid = (hBoxRect.right-hBoxRect.left)/60;
	int rad = (hBoxRect.right-hBoxRect.left)/30;

	for (int i = 0; i <= 4; i++) {
		int pY = hBoxRect.top+ytOff+(hBoxRect.bottom-hBoxRect.top-ybOff-ytOff)*i/4;
		MoveToEx(hBufDC, hBoxRect.left+8, pY, NULL);
		LineTo(hBufDC, hBoxRect.right-8, pY);
	}

	DeleteObject(tracePen);
	DeleteObject(boxBrush);

	HBRUSH sliderBrush = CreateSolidBrush(RGB(240, 240, 240));
	HPEN sliderPen = CreatePen(PS_SOLID, 1, RGB(180, 180, 180));
	SelectObject(hBufDC, sliderBrush); 
	SelectObject(hBufDC, sliderPen);

	HFONT hSubFnt = CreateFont(fHeight*10/14, FW_NORMAL, false);
	HFONT hDefFnt = (HFONT)SelectObject(hBufDC, hSubFnt);
	TEXTMETRIC tm = {0};
	GetTextMetrics(hDC, &tm);

	for (int i = 0; i < 11; i++) {
		int pX = hBoxRect.left+(hBoxRect.right-hBoxRect.left)*(i+1)/12;
		Rectangle(hBufDC, pX-scWid, hBoxRect.top+ytOff, pX+scWid, hBoxRect.bottom-ybOff);
		LPRECT trct = getRect(pX-20, hBoxRect.bottom-ybOff+5, pX+20, hBoxRect.bottom);
		int tslen = wcslen(eqFreq[i])+wcslen(eqHz[i])+1;
		wchar_t ts[10];
		for (int x = 0; x < wcslen(eqFreq[i]); x++) {
			ts[x] = eqFreq[i][x];
		}
		ts[wcslen(eqFreq[i])] = 13;
		for (int x = 0; x < wcslen(eqHz[i]); x++) {
			ts[wcslen(eqFreq[i])+1+x] = eqHz[i][x];
		}
		DrawText(hBufDC, ts, tslen, trct, DT_CENTER|DT_TOP);
		delete trct;
	}

	DeleteObject(sliderBrush);
	DeleteObject(sliderPen);
	SelectObject(hBufDC, hDefFnt);
	DeleteObject(hSubFnt);

	HBRUSH preBrush = CreateSolidBrush(RGB(200, 100, 100));
	HBRUSH ballBrush = CreateSolidBrush(RGB(100, 120, 150));
	HPEN ballPen = CreatePen(PS_SOLID, 1, RGB(60, 60, 60));
	SelectObject(hBufDC, preBrush); 
	SelectObject(hBufDC, ballPen);

	for (int i = 0; i < 11; i++) {
		int pX = hBoxRect.left+(hBoxRect.right-hBoxRect.left)*(i+1)/12;
		int dt = eq.preamp;
		if (i > 0) {		
			dt = eq.data[i-1];
		}
		int pY = hBoxRect.top+ytOff+(hBoxRect.bottom-hBoxRect.top-ytOff-ybOff)*dt/63;
		Ellipse(hBufDC, pX-rad, pY-rad, pX+rad, pY+rad);
		if (i == 0) {
			SelectObject(hBufDC, ballBrush); 
		}
	}

	DeleteObject(preBrush);
	DeleteObject(ballBrush);
	DeleteObject(ballPen);

	BitBlt(hDC, 0, 0, get_devWid(), get_devHei(), hBufDC, 0, 0, SRCCOPY);

	SelectObject(hBufDC, hBufBmpOld);
	DeleteObject(hBufBmp);
	DeleteDC(hBufDC);

}

int CWndEqualizer::MenuProc(int uMsg) {

	if (uMsg == WBR_MENUSELECTED) {
		// 0:Menu close
		if (Menu.ItemIndex == 0) {
			StopMenu();
		}
		
		// 1:Add as preset
		if (Menu.ItemIndex == 1) {
		
			StopMenu();
			if (DialogBox(player()->hInstance, MAKEINTRESOURCE(DLG_ADDPRESET), hWnd, (DLGPROC)CDlgAddPresetProc) == 1) {
				if (wcslen(AddPresetName) > 0) {
					EQPRESET * newPreset = new EQPRESET[presetCount+1];
					for (int i = 0; i < presetCount; i++) {
						newPreset[i] = preset[i];
					}
					if (preset != NULL) {
						delete [] preset;
					}
					preset = newPreset;
						
					newPreset[presetCount].eq = eq;
					wcscpy(newPreset[presetCount].name, AddPresetName);
					presetCount++;
				}
			}
					
		}

		// 2:Remove preset
		if (Menu.ItemIndex == 2) {
			Menu.Data[2].Checked = !Menu.Data[2].Checked;
		}

		// 3:Reset equalizer
		if (Menu.ItemIndex == 3) {

			StopMenu();
			eq.fEnable = false;
			for (int i = 0; i < 10; i++) {
				eq.data[i] = 31;
			}
			eq.preamp = 31;
			MAP_SetEqualizer(player()->MAP, &eq);

		}

		// Presets
		if (Menu.ItemIndex > 3) {

			if (!Menu.Data[1].Checked) {

				StopMenu();
				eq = preset[Menu.ItemIndex-4].eq;
				eq.fEnable = true;
				MAP_SetEqualizer(player()->MAP, &eq);
				InvalidateRect(hWnd, NULL, false);

			} else {

				StopMenu();

				if (presetCount > 1) {
					EQPRESET * newPreset = new EQPRESET[presetCount-1];
					int x = 0;
					for (int i = 0; i < presetCount; i++) {
						if (i != Menu.ItemIndex-4) {
							newPreset[x] = preset[i];
							x++;
						}
					}
					delete [] preset;
					preset = newPreset;
				} else {
					delete [] preset;
					preset = NULL;
				}
				presetCount--;

			}
		}

	}
	return 0;

}

void CWndEqualizer::StartMenu() {

	ClearWbrList(&Menu);
	Menu.Count = 4+presetCount;
	Menu.Data = new WBRITEM[4+presetCount];
	Menu.Data[0] = MenuClose;
	Menu.Data[1] = MenuPresetAdd;
	Menu.Data[2] = MenuPresetDel;
	Menu.Data[3] = MenuPresetClr;
	for (int i = 0; i < presetCount; i++) {
		SetWbrItem(&Menu.Data[4+i], preset[i].name, ICO_PRESET, false, false, false);
	}

	CreateMenuBar(_str(STR_CANCEL), _str(STR_OK));

	ShowingMenu = true;
	lpMenu->Start(&Menu);
	Menu.ItemIndex = -1;
	Menu.OffY = 0;
	InvalidateRect(hWnd, NULL, false);

}

void CWndEqualizer::StopMenu() {
	
	CreateMenuBar(_str(STR_PRESETS), _str(STR_OK));

	ShowingMenu = false;
	InvalidateRect(hWnd, NULL, false);

}

void CWndEqualizer::Close() {
	DestroyWindow(hWnd);
}

void CWndEqualizer::OnKeyPress(int nKey) {

	if (nKey == VK_RIGHT) {
		selBand++;
		if (selBand > 10) {
			selBand = 0;
		}
		InvalidateRect(hWnd, NULL, false);
	}
	
	if (nKey == VK_LEFT) {
		selBand--;
		if (selBand < 0) {
			selBand = 10;
		}
		InvalidateRect(hWnd, NULL, false);
	}

	if (selBand >= 0) {
		if (nKey == VK_DOWN) {
			if (selBand == 0) {
				eq.preamp += 2;
				if (eq.preamp > 63) {
					eq.preamp = 63;
				}
			} else {
				eq.data[selBand-1] += 2;
				if (eq.data[selBand-1] > 63) {
					eq.data[selBand-1] = 63;
				}
			}
			MAP_SetEqualizer(player()->MAP, &eq);
			InvalidateRect(hWnd, NULL, false);
		}
		if (nKey == VK_UP) {
			if (selBand == 0) {
				eq.preamp -= 2;
				if (eq.preamp < 0) {
					eq.preamp = 0;
				}
			} else {
				eq.data[selBand-1] -= 2;
				if (eq.data[selBand-1] < 0) {
					eq.data[selBand-1] = 0;
				}
			}
			MAP_SetEqualizer(player()->MAP, &eq);
			InvalidateRect(hWnd, NULL, false);
		}
	}
}

LRESULT CWndEqualizer::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {

		case WM_CREATE:
			OnCreate();
            break;

		case WM_LBUTTONDOWN:
			if (ShowingMenu) {  lpMenu->OnMouseDown(LOWORD(lParam), HIWORD(lParam));   }
			else             {  OnMouseDown(LOWORD(lParam), HIWORD(lParam));           }
			break;

		case WM_MOUSEMOVE:
			if (ShowingMenu) {  lpMenu->OnMouseMove(LOWORD(lParam), HIWORD(lParam));   }
			else             {  OnMouseMove(LOWORD(lParam), HIWORD(lParam));           }
			break;
		
		case WM_LBUTTONUP:
			if (ShowingMenu) {  lpMenu->OnMouseUp(LOWORD(lParam), HIWORD(lParam));     }
			else             {  OnMouseUp(LOWORD(lParam), HIWORD(lParam));             }
			break;

		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			
			if (wmId == IDM_LSOFT) {

				if (!ShowingMenu) {
					StartMenu();
				} else {
					StopMenu();
				}

			}

			if (wmId == IDM_RSOFT) {
			
				if (!ShowingMenu) {
					Close();
				} else {
					MenuProc(WBR_MENUSELECTED);
				}
			}

			if (wmId == IDOK) {
				Close();
			}

			break;
		}

		case WM_ERASEBKGND:
			return 0;

		case WM_ACTIVATE:
			if ((wParam == WA_INACTIVE) && ((HWND)lParam == player()->lpWndMain->hWnd)) {
				DestroyWindow(hWnd);
			} else {
				DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			break;

		case WM_DESTROY: {
			CommandBar_Destroy(hWndMenuBar);
			wchar_t fName[MAX_PATH];
			wsprintf(fName, L"%spreset.eql", player()->FilesPath);
			HANDLE fl = CreateFile(fName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			DWORD n;
			WriteFile(fl, &presetCount, 2, &n, NULL);
			for (int i = 0; i < presetCount; i++) {
				int ibuf = wcslen(preset[i].name);
				WriteFile(fl, &ibuf, 2, &n, NULL);
				WriteFile(fl, preset[i].name, ibuf*sizeof(wchar_t), &n, NULL);
				WriteFile(fl, &preset[i].eq.preamp, 1, &n, NULL);
				for (int x = 0; x < 10; x++) {
					WriteFile(fl, &preset[i].eq.data[x], 1, &n, NULL);
				}
			}
			delete [] preset;
			CloseHandle(fl);
			delete lpMenu;
			break;
		}

 		case WM_KEYDOWN: {
			if (ShowingMenu) {
				bool firstPress = ((lParam & 0x40000000) == 0);
				lpMenu->OnKeyDown(firstPress, wParam);
			} else {
				OnKeyPress(wParam);
			}
			break;
		}

		case WM_KEYUP:
			if (ShowingMenu) {
				lpMenu->OnKeyUp(wParam);
			}
			break;

		case WM_PAINT: {
			HDC hDC;
			PAINTSTRUCT ps;
			hDC = BeginPaint(hWnd, &ps);
			if (ShowingMenu) {  lpMenu->OnPaint(hDC);        }
			else             {  OnPaint(hDC);                }
            EndPaint(hWnd, &ps);
            break;
		}

		default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
	
	}
	return 0;
}

LRESULT CWndEqualizer::DlgAddPresetProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		case WM_INITDIALOG:
			SetWindowText(dhWnd, _str(STR_ADDASPRESET));
			SetDlgItemText(dhWnd, LBL_PRESETNAME, _str(STR_ENTERTHENAMEOFTH));
			SetDlgItemText(dhWnd, BTN_ADDOK, _str(STR_OK));
			SetFocus(GetDlgItem(dhWnd, EDT_ADDNAME));
			break;
		
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			if (wmId == BTN_ADDOK) {
				memset(AddPresetName, 0, 20*sizeof(wchar_t));
				GetDlgItemText(dhWnd, EDT_ADDNAME, AddPresetName, 30);
				EndDialog(dhWnd, 1);
			}
			break;
		}

		default:
            return DefWindowProc(dhWnd, uMsg, wParam, lParam);
	
	}
	return 0;
}

LRESULT CALLBACK CWndEqualizerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (uMsg == WM_CREATE) {
		player()->lpWndEqualizer->hWnd = hWnd;
	}

	return player()->lpWndEqualizer->WndProc(uMsg, wParam, lParam);

}

LRESULT CALLBACK CDlgAddPresetProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (uMsg == WM_INITDIALOG) {
		player()->lpWndEqualizer->dhWnd = hWnd;
	}

	return player()->lpWndEqualizer->DlgAddPresetProc(uMsg, wParam, lParam);

}

int CWndEqualizerMenuProc(int uMsg) {
	
	return player()->lpWndEqualizer->MenuProc(uMsg);

}

CWndEqualizer::~CWndEqualizer() {

	delete lpMenu;
}
