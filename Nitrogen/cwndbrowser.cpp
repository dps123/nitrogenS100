#include "stdafx.h"
#include "main.h"

CWndBrowser::CWndBrowser() {

	lpMenu = NULL;

}

void CWndBrowser::CreateMenuBar(LPWSTR lText, LPWSTR rText) {
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

ATOM CWndBrowser::InitClass() {
	WNDCLASS wc;
	wc.style  = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)CWndBrowserProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = player()->hInstance;
	wc.hIcon = LoadIcon(player()->hInstance, MAKEINTRESOURCE(ICO_MAIN));
	wc.hCursor = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"CNtgWndBrowser";
	return RegisterClass(&wc);
}

bool CWndBrowser::Show(LPWSTR lpTitle, LPWSTR lSoft, LPWSTR rSoft, WBRLIST * List, WBRPROC msgproc) {
	MsgProc = msgproc;
	lpList = List;
	wcscpy(lpSoft[0], lSoft);
	wcscpy(lpSoft[1], rSoft);
	HWND h = CreateWindow(L"CNtgWndBrowser", lpTitle, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, player()->hInstance, NULL);
	if (!h) {
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
	Constrain();
	Repaint();
	UpdateWindow(hWnd);
	return true;	
}

void CWndBrowser::OnCreate() {

	hWndMenuBar = NULL;
	CreateMenuBar(lpSoft[0], lpSoft[1]);

	hapEnabled = false;
	ShowingMenu = false;
	lpMenu = new CMenu(hWnd, CWndBrowserMenuProc);

	Constrain();

}

void CWndBrowser::OnMouseDown(int px, int py) {

	KillTimer(hWnd, TMR_HAPTICS);
	hapTick = GetTickCount();
	hapYCord = py;
	disableClick = hapEnabled;
	hapEnabled = false;
	
	if (py >= getBrowserProp(IBP_TITLEHEIGHT)) {
		bool Scroller = (get_devHei()-getBrowserProp(IBP_TITLEHEIGHT) < lpList->Count*getBrowserProp(IBP_ITEMHEIGHT));
		if (Scroller && (px >= get_devWid()-getBrowserProp(IBP_SCROLLERSIZE))) {
			int scHei = max(getBrowserProp(IBP_SCROLLERSIZE)/2, (get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))*(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))/(lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)));
			int scTop = getBrowserProp(IBP_TITLEHEIGHT)+(int)((get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-scHei)*((double)lpList->OffY/(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT))));
			if ((py >= scTop) && (py < scTop+scHei)) {
				ClickMode = CLK_SCROLLER;
				pY = py-scTop;
			} else {
				ClickMode = CLK_SCROLLAREA;
				pY = py;
				if (py < scTop) {
					saUp = true;
					lpList->OffY = lpList->OffY+(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))*0.8;
				} else if (py > scTop+scHei) {
					saUp = false;
					lpList->OffY = lpList->OffY-(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))*0.8;			
				}
				if (lpList->OffY < get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)) { lpList->OffY = get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT); }
				if (lpList->OffY > 0) { lpList->OffY = 0; }

				SetTimer(hWnd, TMR_SCROLLAREA, 500, NULL);
			}
			Repaint();
		} else {	
			longTap = false;
			SetTimer(hWnd, TMR_LONGTAP, 500, NULL);
			ClickMode = CLK_DRAG;
			dragging = false;
			oldOffY = lpList->OffY;
			oldPY = py;
			pY = py;
			lpList->ItemIndex = (int)((pY-lpList->OffY-getBrowserProp(IBP_TITLEHEIGHT))/getBrowserProp(IBP_ITEMHEIGHT));
			if (lpList->ItemIndex >= lpList->Count) {
				lpList->ItemIndex = -1;
			}
			Repaint();
		}
	} else {
		ClickMode = CLK_TITLE;
		OnMouseMove(px, py);
	}

}

void CWndBrowser::OnMouseMove(int px, int py) {
	int offset = 0;
	if (hWndMenuBar) {
		RECT rcMenuBar;
		GetWindowRect(hWndMenuBar, &rcMenuBar);
		offset=rcMenuBar.bottom-rcMenuBar.top;
	}

	if (ClickMode == CLK_DRAG) {
		pY = py;
		if ((!dragging) && (abs(oldPY-py) > 10)) {
			dragging = true;
			hapEnabled = true;
		}
		if (dragging) {
			lpList->OffY = oldOffY-oldPY+py;
			if (lpList->OffY < get_devHei()-2*offset-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)) { 
				lpList->OffY = get_devHei()-2*offset-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT); 
			}
			if (lpList->OffY > 0) { lpList->OffY = 0; }
			Repaint();
		}
	} else if (ClickMode == CLK_SCROLLER) {
		int scHei = max(getBrowserProp(IBP_SCROLLERSIZE)/2, (get_devHei()-2*offset-getBrowserProp(IBP_TITLEHEIGHT))*(get_devHei()-2*offset-getBrowserProp(IBP_TITLEHEIGHT))/(lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)));

		lpList->OffY = (double)(py-pY-getBrowserProp(IBP_TITLEHEIGHT))/(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-scHei)*(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT));
		if (lpList->OffY < get_devHei()-2*offset-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)) { 
			lpList->OffY = get_devHei()-2*offset-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT); 
		}
		if (lpList->OffY > 0) { lpList->OffY = 0; }
		Repaint();
	} else if (ClickMode == CLK_SCROLLAREA) {
		pY = py;
	}
}

void CWndBrowser::OnMouseUp(int px, int py) {

	if (ClickMode == CLK_DRAG) {
		KillTimer(hWnd, TMR_LONGTAP);
		if ((dragging == false) && (!longTap)) {
			lpList->ItemIndex = (int)((py-lpList->OffY-getBrowserProp(IBP_TITLEHEIGHT))/getBrowserProp(IBP_ITEMHEIGHT));
			if (lpList->ItemIndex >= lpList->Count) {
				lpList->ItemIndex = -1;
			}
			if ((px < getBrowserProp(IBP_ITEMHEIGHT)) && (lpList->CheckEnabled)) {
				lpList->Data[lpList->ItemIndex].Checked = !lpList->Data[lpList->ItemIndex].Checked;
			} else {
				if (!disableClick) {
					MsgProc(WBR_SELECTED);
				}
			}
		} else if (dragging) {
			DWORD deltaTick = GetTickCount()-hapTick;
			int deltaY = py-hapYCord;
			if (player()->lpConfig->cf.hapEnabled && (((double)deltaY/deltaTick > (double)player()->lpConfig->cf.hapMinGesture/100) || ((double)deltaY/deltaTick < -(double)player()->lpConfig->cf.hapMinGesture/100))) {
				hapAcc = (double)deltaY/deltaTick*(double)player()->lpConfig->cf.hapInitialSpeed;
				hapFPY = lpList->OffY;
				hapDir = (deltaY > 0);
				SetTimer(hWnd, TMR_HAPTICS, player()->lpConfig->cf.hapRefreshInterval, NULL);
			} else {
				hapEnabled = false;
			}
		}
	} else if ((ClickMode == CLK_TITLE) && (py < getBrowserProp(IBP_TITLEHEIGHT))) {
		if (px<5+(getIcons()->IconSize+20)-10) {
			MsgProc(WBR_OK);
		} else {
			if (px<5+2*(getIcons()->IconSize+20)-10) {
				MsgProc(WBR_RSOFTKEY);
			} else {
				if (px<5+3*(getIcons()->IconSize+20)-10) {
					MsgProc(WBR_LSOFTKEY);
				} else {
					MsgProc(WBR_CLICKTITLE);
				}
			}
		}
	} else if (ClickMode == CLK_SCROLLAREA) {
		KillTimer(hWnd, TMR_SCROLLAREA);
	}
	ClickMode = CLK_NONE;
	Repaint();
}

void CWndBrowser::OnKeyDown(bool firstPress, int nKey) {

	if (nKey == VK_UP) {
	
		lpList->ItemIndex--;
		if (lpList->ItemIndex < 0) {
			lpList->ItemIndex = lpList->Count-1;
		}
		Constrain();
		Repaint();

	} else if (nKey == VK_DOWN) {
	
		lpList->ItemIndex++;
		if (lpList->ItemIndex >= lpList->Count) {
			if (lpList->Count > 0) {
				lpList->ItemIndex = 0;
			} else {
				lpList->ItemIndex = -1;
			}
		}

		Constrain();
		Repaint();
	
	} else if ((nKey == VK_ACTION) || (nKey == VK_RIGHT)) {

		if (firstPress) {

			keyProc = false;
			SetTimer(hWnd, TMR_LONGKEYPRESS, 500, NULL);
		
		}

	} else if (nKey == VK_LEFT) {

		MsgProc(WBR_CLICKTITLE);
		Repaint();

	} else if (((nKey >= 65) && (nKey <= 90)) || ((nKey >= 97) && (nKey <= 122))) {
	
		int cKey = nKey;
		if ((cKey >= 65) && (cKey <= 90)) {
			cKey += 32;
		}
		
		bool fnd = false;
		for (int i = lpList->ItemIndex+1; i < lpList->Count; i++) {
			if (lpList->Data[i].TextLength > 0) {
				int bKey = lpList->Data[i].Text[0];
				if ((bKey >= 65) && (bKey <= 90)) {
					bKey += 32;
				}
				if (bKey == cKey) {
					fnd = true;
					lpList->ItemIndex = i;
					break;
				}
			}
		}

		if (!fnd) {

			for (int i = 0; i < lpList->ItemIndex; i++) {
				if (lpList->Data[i].TextLength > 0) {
					int bKey = lpList->Data[i].Text[0];
					if ((bKey >= 65) && (bKey <= 90)) {
						bKey += 32;
					}
					if (bKey == cKey) {
						fnd = true;
						lpList->ItemIndex = i;
						break;
					}
				}
			}
		}

		if (fnd) {
			Constrain();
			Repaint();
		}

	}
}

void CWndBrowser::OnKeyUp(int nKey) {

	if (((nKey == VK_ACTION) || (nKey == VK_RIGHT)) && (!keyProc)) {

		KillTimer(hWnd, TMR_LONGKEYPRESS);
		MsgProc(WBR_SELECTED);
		Repaint();	

	}
}

void CWndBrowser::SetSoftkey(int nSoft, LPWSTR newText) {
	wcscpy(lpSoft[nSoft], newText);
	if (!ShowingMenu) {
		CreateMenuBar(lpSoft[0], lpSoft[1]);
	}
}

void CWndBrowser::SetTitle(LPWSTR newTitle) {
	SetWindowText(hWnd, newTitle);
}

void CWndBrowser::ChangeList(WBRLIST * newList) {
	KillTimer(hWnd, TMR_HAPTICS);
	hapEnabled = false;
	lpList = newList;
	Constrain();
	Repaint();
}

void CWndBrowser::Repaint() {
	InvalidateRect(hWnd, NULL, false);
}

void CWndBrowser::OnPaint(HDC hDC) {

	HBRUSH bgBrush = CreateSolidBrush(getBrowserColor(IBC_ITEMSBG));
	HBRUSH bgSelBrush = CreateSolidBrush(getBrowserColor(IBC_SELITEMBG));
	HBRUSH bgDirBrush = CreateSolidBrush(getBrowserColor(IBC_TITLEBG));
	HBRUSH bgCheckBrush = CreateSolidBrush(getBrowserColor(IBC_CHECKED));

	// Creates offscreen bitmap
	HBITMAP hBmp = CreateBitmap(get_devWid(), get_devHei(), 1, 16, NULL);
	HDC hBufDC = CreateCompatibleDC(NULL);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hBufDC, hBmp);
	
	HPEN lnPen = CreatePen(PS_DASH, 1, getBrowserColor(IBC_LINE));
	int wei = FW_NORMAL;
	if (getBrowserProp(IBP_ITEMFONTBOLD) == 1) {
		wei = FW_BOLD;
	}
	HFONT fnt = CreateFont(getBrowserProp(IBP_ITEMFONTSIZE), wei, hapEnabled && player()->lpConfig->cf.hapDraftQuality);

	bool Scroller = (get_devHei()-getBrowserProp(IBP_TITLEHEIGHT) < lpList->Count*getBrowserProp(IBP_ITEMHEIGHT));

	LPRECT rct = getRect(0, getBrowserProp(IBP_TITLEHEIGHT), get_devWid()-Scroller*getBrowserProp(IBP_SCROLLERSIZE), get_devHei());
	FillRect(hBufDC, rct, bgBrush);
	delete rct;

	HPEN hPenOld = NULL;
	if (Scroller) {
		HBRUSH bgScrollerBrushN = CreateSolidBrush(getBrowserColor(IBC_SCROLLERBG));
		HBRUSH bgScrollerBrushC = CreateSolidBrush(MixColors(getBrowserColor(IBC_SCROLLERBG), RGB(0,0,0), 0.3));
		HBRUSH fgScrollerBrushL = CreateSolidBrush(MixColors(getBrowserColor(IBC_SCROLLER), RGB(255, 255, 255), 0.05+0.15*(ClickMode == CLK_SCROLLER)));
		HBRUSH fgScrollerBrushS = CreateSolidBrush(MixColors(MixColors(getBrowserColor(IBC_SCROLLER), RGB(0, 0, 0), 0.05), RGB(255,255,255), 0.12*(ClickMode == CLK_SCROLLER)));
	
		int scHei = max(getBrowserProp(IBP_SCROLLERSIZE)/2, (get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))*(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))/(lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)));
		int scTop = getBrowserProp(IBP_TITLEHEIGHT)+(int)((get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-scHei)*((double)lpList->OffY/(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT))));

		LPRECT scrollerClientRect = getRect(get_devWid()-getBrowserProp(IBP_SCROLLERSIZE), getBrowserProp(IBP_TITLEHEIGHT), get_devWid(), get_devHei());
		LPRECT scrollerClientRectUp = getRect(scrollerClientRect->left, scrollerClientRect->top, scrollerClientRect->right, scTop);
		LPRECT scrollerClientRectDown = getRect(scrollerClientRect->left, scTop+scHei, scrollerClientRect->right, scrollerClientRect->bottom);

		if ((ClickMode == CLK_SCROLLAREA) && (saUp)) {
			FillRect(hBufDC, scrollerClientRectUp, bgScrollerBrushC);
		} else {
			FillRect(hBufDC, scrollerClientRectUp, bgScrollerBrushN);
		}

		if ((ClickMode == CLK_SCROLLAREA) && (!saUp)) {
			FillRect(hBufDC, scrollerClientRectDown, bgScrollerBrushC);
		} else {
			FillRect(hBufDC, scrollerClientRectDown, bgScrollerBrushN);
		}

		HPEN lnScr = CreatePen(PS_SOLID, 1, getBrowserColor(IBC_LINE));

		HPEN hPenOld = (HPEN)SelectObject(hBufDC, lnScr);
		MoveToEx(hBufDC, scrollerClientRect->left, getBrowserProp(IBP_TITLEHEIGHT), NULL);
		LineTo(hBufDC, scrollerClientRect->left, scrollerClientRect->bottom);
		DeleteObject(lnScr);

		LPRECT scrollerRect = getRect(get_devWid()-getBrowserProp(IBP_SCROLLERSIZE)+1, scTop, get_devWid()-getBrowserProp(IBP_SCROLLERSIZE)/2, scTop+scHei);
		FillRect(hBufDC, scrollerRect, fgScrollerBrushL);
		delete scrollerRect;
		scrollerRect = getRect(get_devWid()-getBrowserProp(IBP_SCROLLERSIZE)/2, scTop, get_devWid(), scTop+scHei);
		FillRect(hBufDC, scrollerRect, fgScrollerBrushS);
		delete scrollerRect;
		
		HPEN ln = CreatePen(PS_SOLID, 1, MixColors(getBrowserColor(IBC_SCROLLER), RGB(255,255,255), 0.2));
		SelectObject(hBufDC, ln);
		for (int x = -4; x < 5; x++) {
			int py = scTop+scHei/2+x*4;
			if ((py > scTop) && (py < scTop+scHei)) {
				MoveToEx(hBufDC, scrollerClientRect->left+2, py, NULL);
				LineTo(hBufDC, scrollerClientRect->right-2, py);	
			}
		}
		DeleteObject(ln);
		
		delete scrollerClientRect;
		DeleteObject(fgScrollerBrushL);
		DeleteObject(fgScrollerBrushS);
		DeleteObject(bgScrollerBrushN);
		DeleteObject(bgScrollerBrushC);
	}

	HBRUSH bgRightSideBrush = CreateSolidBrush(getBrowserColor(IBC_RIGHTSIDE));
	LPRECT iconRect = getRect(0, 0, getBrowserProp(IBP_ITEMHEIGHT), get_devHei());
	FillRect(hBufDC, iconRect, bgRightSideBrush);
	delete iconRect;
	DeleteObject(bgRightSideBrush);
	
	SetTextColor(hBufDC, getBrowserColor(IBC_ITEMSTEXT));
	SetBkMode(hBufDC, TRANSPARENT);
	
	if (hPenOld == NULL) {
		hPenOld = (HPEN)SelectObject(hBufDC, (HPEN)lnPen);
	} else {
		SelectObject(hBufDC, (HPEN)lnPen);
	}
	HFONT hFntOld = (HFONT)SelectObject(hBufDC, fnt);

	for (int i = 0; i < lpList->Count; i++) {
		if (lpList->OffY+i*getBrowserProp(IBP_ITEMHEIGHT) > get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)) {
			break;
		} else if (lpList->OffY+(i+1)*getBrowserProp(IBP_ITEMHEIGHT) > 0) {
			if (i == lpList->ItemIndex) {
				int inc = 0;
				if (i > 0) { inc = 1; }
				LPRECT sr = getRect(0, getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+i*getBrowserProp(IBP_ITEMHEIGHT)+inc, get_devWid()-Scroller*getBrowserProp(IBP_SCROLLERSIZE), getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+(i+1)*getBrowserProp(IBP_ITEMHEIGHT));
				FillRect(hBufDC, sr, bgSelBrush);
				SetTextColor(hBufDC, getBrowserColor(IBC_SELITEMTEXT));
			}
			if (lpList->Data[i].Checked) {
				LPRECT rcc = getRect(2, getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+i*getBrowserProp(IBP_ITEMHEIGHT)+2, getBrowserProp(IBP_ITEMHEIGHT)-2, getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+(i+1)*getBrowserProp(IBP_ITEMHEIGHT)-2);
				FillRect(hBufDC, rcc, bgCheckBrush);
			}
			LPRECT rct = getRect(6+getBrowserProp(IBP_ITEMHEIGHT), getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+i*getBrowserProp(IBP_ITEMHEIGHT), get_devWid()-5-Scroller*getBrowserProp(IBP_SCROLLERSIZE), getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+(i+1)*getBrowserProp(IBP_ITEMHEIGHT));
			if ((getIcons()->Count > lpList->Data[i].IconIndex) && (getIcon(lpList->Data[i].IconIndex)->Exists)) {
				TransparentBlt(hBufDC, (getBrowserProp(IBP_ITEMHEIGHT)-getIcons()->IconSize)/2, getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+getBrowserProp(IBP_ITEMHEIGHT)*i+(getBrowserProp(IBP_ITEMHEIGHT)-getIcons()->IconSize)/2, getIcons()->IconSize, getIcons()->IconSize, getIcon(lpList->Data[i].IconIndex)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
			}
			if ((lpList->Data[i].Arrow) && (getIcons()->Count > ICO_ARROW) && (getIcon(ICO_ARROW)->Exists)) {
				TransparentBlt(hBufDC, get_devWid()-getIcons()->IconSize-4-Scroller*getBrowserProp(IBP_SCROLLERSIZE), getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+getBrowserProp(IBP_ITEMHEIGHT)*i+(getBrowserProp(IBP_ITEMHEIGHT)-getIcons()->IconSize)/2, getIcons()->IconSize, getIcons()->IconSize, getIcon(ICO_ARROW)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
				rct->right = get_devWid()-getIcons()->IconSize-6-Scroller*getBrowserProp(IBP_SCROLLERSIZE);
			}
			DrawText(hBufDC, lpList->Data[i].Text, lpList->Data[i].TextLength, rct, DT_VCENTER|DT_LEFT|DT_WORD_ELLIPSIS|DT_NOPREFIX);
			MoveToEx(hBufDC, 0, getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+(i+1)*getBrowserProp(IBP_ITEMHEIGHT), NULL);
			LineTo(hBufDC, get_devWid()-Scroller*getBrowserProp(IBP_SCROLLERSIZE), getBrowserProp(IBP_TITLEHEIGHT)+lpList->OffY+(i+1)*getBrowserProp(IBP_ITEMHEIGHT));
			if (i == lpList->ItemIndex) {
				SetTextColor(hBufDC, getBrowserColor(IBC_ITEMSTEXT));
			}
			delete rct;
		}
	}
	DeleteObject(fnt);
	DeleteObject(lnPen);
	DeleteObject(bgBrush);
	DeleteObject(bgSelBrush);
	DeleteObject(bgCheckBrush);
	
	rct = getRect(0, 0, get_devWid(), getBrowserProp(IBP_TITLEHEIGHT));
	FillRect(hBufDC, rct, bgDirBrush);
	delete rct;
	DeleteObject(bgDirBrush);
	lnPen = CreatePen(PS_SOLID, 1, getBrowserColor(IBC_LINE));
	SelectObject(hBufDC, (HPEN)lnPen);
	MoveToEx(hBufDC, 0, getBrowserProp(IBP_TITLEHEIGHT), NULL);
	LineTo(hBufDC, get_devWid(), getBrowserProp(IBP_TITLEHEIGHT));
	DeleteObject(lnPen);
	
	// Рисуем иконки управления
	if ((getIcons()->Count > ICO_CLOSEMENU) && (getIcon(ICO_CLOSEMENU)->Exists)) {
		TransparentBlt(hBufDC, 5+0*(getIcons()->IconSize+20), (getBrowserProp(IBP_TITLEHEIGHT)-getIcons()->IconSize)/2, getIcons()->IconSize, getIcons()->IconSize, getIcon(ICO_CLOSEMENU)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
	}
	if ((getIcons()->Count > lpIconMode) && (getIcon(lpIconMode)->Exists)) {
		TransparentBlt(hBufDC, 5+1*(getIcons()->IconSize+20), (getBrowserProp(IBP_TITLEHEIGHT)-getIcons()->IconSize)/2, getIcons()->IconSize, getIcons()->IconSize, getIcon(lpIconMode)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
	}
	if ((getIcons()->Count > ICO_SETTINGS) && (getIcon(ICO_SETTINGS)->Exists)) {
		TransparentBlt(hBufDC, 5+2*(getIcons()->IconSize+20), (getBrowserProp(IBP_TITLEHEIGHT)-getIcons()->IconSize)/2, getIcons()->IconSize, getIcons()->IconSize, getIcon(ICO_SETTINGS)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
	}
	

	if ((lpList->TitleIcon > -1) && (getIcons()->Count > lpList->TitleIcon) && (getIcon(lpList->TitleIcon)->Exists)) {
		//rct = getRect(5, 0, get_devWid()-8-getIcons()->IconSize, getBrowserProp(IBP_TITLEHEIGHT));
		rct = getRect(5+2*(getIcons()->IconSize+20)+getIcons()->IconSize+3, 0, get_devWid()-8-getIcons()->IconSize, getBrowserProp(IBP_TITLEHEIGHT));
		TransparentBlt(hBufDC, get_devWid()-5-getIcons()->IconSize, (getBrowserProp(IBP_TITLEHEIGHT)-getIcons()->IconSize)/2, getIcons()->IconSize, getIcons()->IconSize, getIcon(lpList->TitleIcon)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
	} else {
		//rct = getRect(5, 0, get_devWid()-5, getBrowserProp(IBP_TITLEHEIGHT));
		rct = getRect(5+2*(getIcons()->IconSize+20)+getIcons()->IconSize+3, 0, get_devWid()-5, getBrowserProp(IBP_TITLEHEIGHT));
	}

	wei = FW_NORMAL;
	if (getBrowserProp(IBP_TITLEFONTBOLD) == 1) {
		wei = FW_BOLD;
	}
	fnt = CreateFont(getBrowserProp(IBP_TITLEFONTSIZE), wei, hapEnabled && player()->lpConfig->cf.hapDraftQuality);
	SelectObject(hBufDC, fnt);
	SetTextColor(hBufDC, getBrowserColor(IBC_TITLETEXT));
	DrawText(hBufDC, lpList->Title, wcslen(lpList->Title), rct, DT_VCENTER|DT_RIGHT|DT_NOPREFIX);
	delete rct;
	DeleteObject(fnt);

	BitBlt(hDC, 0, 0, get_devWid(), get_devHei(), hBufDC, 0, 0, SRCCOPY);
	SelectObject(hBufDC, hBmpOld);
	SelectObject(hBufDC, hPenOld);
	SelectObject(hBufDC, hFntOld);
	DeleteObject(hBmp);
	DeleteDC(hBufDC);

}

void CWndBrowser::OnTimer(int tmrId) {

	if (tmrId == TMR_HAPTICS) {
		if (hapDir) {
			hapAcc = hapAcc-(double)player()->lpConfig->cf.hapDeacceleration/10;
		} else {
			hapAcc = hapAcc+(double)player()->lpConfig->cf.hapDeacceleration/10;
		}
		hapFPY = hapFPY+hapAcc;
		lpList->OffY = hapFPY;
		if (lpList->OffY < get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-getBrowserProp(IBP_ITEMHEIGHT)*lpList->Count) {
			lpList->OffY = get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-getBrowserProp(IBP_ITEMHEIGHT)*lpList->Count;
			KillTimer(hWnd, TMR_HAPTICS);
			hapEnabled = false;
		}
		if (lpList->OffY > 0) {
			lpList->OffY = 0;
			KillTimer(hWnd, TMR_HAPTICS);
			hapEnabled = false;
		}
		if ((hapDir && (hapAcc < 0)) || (!hapDir && (hapAcc > 0))) {
			KillTimer(hWnd, TMR_HAPTICS);
			hapEnabled = false;
		}
		Repaint();
		UpdateWindow(hWnd);
	}

	if (tmrId == TMR_LONGTAP) {
		KillTimer(hWnd, TMR_LONGTAP);
		if (!dragging) {
			longTap = true;
			MsgProc(WBR_LONGTAP);
			Repaint();
		}
	}

	if (tmrId == TMR_SCROLLAREA) {	
		int scHei = max(getBrowserProp(IBP_SCROLLERSIZE)/2, (get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))*(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))/(lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)));
		int scTop = getBrowserProp(IBP_TITLEHEIGHT)+(int)((get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-scHei)*((double)lpList->OffY/(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT))));

		if (saUp && (pY < scTop)) {
			lpList->OffY = lpList->OffY+(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))*0.8;
		} else if (!saUp && (pY > scTop+scHei)) {
			lpList->OffY = lpList->OffY-(get_devHei()-getBrowserProp(IBP_TITLEHEIGHT))*0.8;			
		}
		if (lpList->OffY < get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)) { lpList->OffY = get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-lpList->Count*getBrowserProp(IBP_ITEMHEIGHT); }
		if (lpList->OffY > 0) { lpList->OffY = 0; }

		Repaint();
		SetTimer(hWnd, TMR_SCROLLAREA, 80, NULL);
	}

	if (tmrId == TMR_LONGKEYPRESS) {
	
		KillTimer(hWnd, TMR_LONGKEYPRESS);
		keyProc = true;
		MsgProc(WBR_LONGTAP);
		Repaint();

	}
}

void CWndBrowser::Close() {
	DestroyWindow(hWnd);
}

int CWndBrowser::MenuProc(int uMsg) {
	if (uMsg == WBR_MENUSELECTED) {
		MsgProc(WBR_MENUSELECTED);
	}
	return 0;
}

void CWndBrowser::Constrain() {
	
	if (lpList->ItemIndex >= 0) {
		if (lpList->OffY < -lpList->ItemIndex*getBrowserProp(IBP_ITEMHEIGHT)+10) {
			lpList->OffY = -lpList->ItemIndex*getBrowserProp(IBP_ITEMHEIGHT)+10;
		}

		if (lpList->OffY > -(lpList->ItemIndex+1)*getBrowserProp(IBP_ITEMHEIGHT)+get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-10) {
			lpList->OffY = -(lpList->ItemIndex+1)*getBrowserProp(IBP_ITEMHEIGHT)+get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)-10;
		}
	}
	Bound();
}

void CWndBrowser::Bound() {

	if (lpList->OffY < -lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)+get_devHei()-getBrowserProp(IBP_TITLEHEIGHT)) {
		lpList->OffY = -lpList->Count*getBrowserProp(IBP_ITEMHEIGHT)+get_devHei()-getBrowserProp(IBP_TITLEHEIGHT);
	}

	if (lpList->OffY > 0) {
		lpList->OffY = 0;
	}

}

void CWndBrowser::StartMenu(WBRLIST * lMenu) {
	lpMenu->Start(lMenu);
	lpMenu->lpItems->ItemIndex = -1;
	lpMenu->lpItems->OffY = 0;
	ShowingMenu = true;
	CreateMenuBar(_str(STR_CANCEL), _str(STR_OK));
	Repaint();
}

void CWndBrowser::StopMenu() {
	ShowingMenu = false;
	CreateMenuBar(lpSoft[0], lpSoft[1]);
}

LRESULT CWndBrowser::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			if (wmId == IDM_LSOFT) {
				if (ShowingMenu) {
					StopMenu();
				} else {
					MsgProc(WBR_LSOFTKEY);
				}
				Repaint();
			} else if (wmId == IDM_RSOFT) {
				if (ShowingMenu) {
					MsgProc(WBR_MENUSELECTED);
				} else {
					MsgProc(WBR_RSOFTKEY);
				}
				Repaint();
			} else if (wmId == IDOK) {
				MsgProc(WBR_OK);
			} else {
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			break;
		}

		case WM_LBUTTONDOWN: {
			int px = LOWORD(lParam);
			int py = HIWORD(lParam);
			if (ShowingMenu) {
				lpMenu->OnMouseDown(px, py);
			} else {
				OnMouseDown(px, py);
			}
			break;
		}

		case WM_MOUSEMOVE: {
			int px = LOWORD(lParam);
			int py = HIWORD(lParam);
			if (ShowingMenu) {
				lpMenu->OnMouseMove(px, py);
			} else {
				OnMouseMove(px, py);
			}
			break;
		}

		case WM_LBUTTONUP: {
			int px = LOWORD(lParam);
			int py = HIWORD(lParam);		
			if (ShowingMenu) {
				lpMenu->OnMouseUp(px, py);
			} else {
				OnMouseUp(px, py);
			}
			break;
		}

		case WM_KEYDOWN: {
			bool firstPress = ((lParam & 0x40000000) == 0);
			if (ShowingMenu) {
				lpMenu->OnKeyDown(firstPress, wParam);
			} else {
				OnKeyDown(firstPress, wParam);
			}
			break;
		}

		case WM_KEYUP: {
			if (ShowingMenu) {
				lpMenu->OnKeyUp(wParam);
			} else {
				OnKeyUp(wParam);
			}
			break;
		}

		case WM_CREATE: {
			OnCreate();
            break;
		}

		case WM_TIMER: {
			OnTimer(wParam);
			break;
		}

		case WM_ACTIVATE: {
			if ((wParam == WA_INACTIVE) && ((HWND)lParam == player()->lpWndMain->hWnd)) {
				DestroyWindow(hWnd);
			} else {
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
            break;
		}

		case WM_DESTROY: {
			KillTimer(hWnd, TMR_HAPTICS);
			MsgProc(WBR_DESTROY);
			delete lpMenu;
			CommandBar_Destroy(hWndMenuBar);
			break;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
            if (ShowingMenu) {
				lpMenu->OnPaint(hDC);
			} else {
				OnPaint(hDC);
			}
            EndPaint(hWnd, &ps);
            break;
		}

		case WM_ERASEBKGND: {
			return 0;					
		}

		default: {
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	return 0;
	
}

LRESULT CALLBACK CWndBrowserProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CREATE) {
		player()->lpWndBrowser->hWnd = hWnd;
	}
	return player()->lpWndBrowser->WndProc(uMsg, wParam, lParam);
}

int CWndBrowserMenuProc(int uMsg) {
	return player()->lpWndBrowser->MenuProc(uMsg);
}

CWndBrowser::~CWndBrowser() {

	delete lpMenu;

}
