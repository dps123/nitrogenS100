#include "stdafx.h"
#include "main.h"

CMenu::CMenu(HWND owner, MENUPROC msgproc) {

	UpdateMenuSize();
	Owner = owner;
	MsgProc = msgproc;
	menuBG.Exists = false;

}

void CMenu::UpdateMenuSize() {

	menuRect.left = get_devWid()/20;
	menuRect.top = get_devHei()/20;
	menuRect.right = get_devWid()*19/20;
	menuRect.bottom = get_devHei()*19/20;

}

void CMenu::OnKeyDown(bool firstPress, int nKey) {

	if (nKey == VK_UP) {
	
		lpItems->ItemIndex--;
		if (lpItems->ItemIndex < 0) {
			lpItems->ItemIndex = lpItems->Count-1;
		}

		Constrain();
		InvalidateRect(Owner, NULL, false);

	} else if (nKey == VK_DOWN) {
	
		lpItems->ItemIndex++;
		if (lpItems->ItemIndex >= lpItems->Count) {
			if (lpItems->Count > 0) {
				lpItems->ItemIndex = 0;
			} else {
				lpItems->ItemIndex = -1;
			}
		}

		Constrain();
		InvalidateRect(Owner, NULL, false);
	
	} else if (((nKey >= 65) && (nKey <= 90)) || ((nKey >= 97) && (nKey <= 122))) {
	
		int cKey = nKey;
		if ((cKey >= 65) && (cKey <= 90)) {
			cKey += 32;
		}
		
		bool fnd = false;
		for (int i = lpItems->ItemIndex+1; i < lpItems->Count; i++) {
			if (lpItems->Data[i].TextLength > 0) {
				int bKey = lpItems->Data[i].Text[0];
				if ((bKey >= 65) && (bKey <= 90)) {
					bKey += 32;
				}
				if (bKey == cKey) {
					fnd = true;
					lpItems->ItemIndex = i;
					break;
				}
			}
		}

		if (!fnd) {

			for (int i = 0; i < lpItems->ItemIndex; i++) {
				if (lpItems->Data[i].TextLength > 0) {
					int bKey = lpItems->Data[i].Text[0];
					if ((bKey >= 65) && (bKey <= 90)) {
						bKey += 32;
					}
					if (bKey == cKey) {
						fnd = true;
						lpItems->ItemIndex = i;
						break;
					}
				}
			}
		}

		if (fnd) {
			Constrain();		
			InvalidateRect(Owner, NULL, false);
		}

	}

}

void CMenu::OnKeyUp(int nkey) {

	if (nkey == VK_ACTION) {
		MsgProc(WBR_MENUSELECTED);
		InvalidateRect(Owner, NULL, false);			
	}
}

void CMenu::OnMouseDown(int px, int py) {
	if ((py >= menuRect.top) && (py < menuRect.bottom)) {
		bool Scroller = (menuRect.bottom-menuRect.top < lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT));
		if (Scroller && (px >= menuRect.right-getBrowserProp(IBP_SCROLLERSIZE))) {
			int scHei = max(getBrowserProp(IBP_SCROLLERSIZE)/2, (menuRect.bottom-menuRect.top)*(menuRect.bottom-menuRect.top)/(lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT)));
			int scTop = (int)((menuRect.bottom-menuRect.top-scHei)*((double)lpItems->OffY/(menuRect.bottom-menuRect.top-lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT))));
			if ((py-menuRect.top >= scTop) && (py-menuRect.top < scTop+scHei)) {
				ClickMode = CLK_SCROLLER;
				PY = py-scTop-menuRect.top;
			}
		} else {
			ClickMode = CLK_DRAG;
			Dragging = false;
			OldOffY = lpItems->OffY;
			OldPY = py;
			PY = py;
			lpItems->ItemIndex = (int)((PY-lpItems->OffY-menuRect.top)/getBrowserProp(IBP_ITEMHEIGHT));
			if (lpItems->ItemIndex >= lpItems->Count) {
				lpItems->ItemIndex = -1;
			}
		}
	}
	InvalidateRect(Owner, NULL, false);
}

void CMenu::OnMouseMove(int px, int py) {
	if (ClickMode == CLK_SCROLLER) {
		int scHei = max(getBrowserProp(IBP_SCROLLERSIZE)/2, (menuRect.bottom-menuRect.top)*(menuRect.bottom-menuRect.top)/(lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT)));

		lpItems->OffY = (double)(py-PY-menuRect.top)/(menuRect.bottom-menuRect.top-scHei)*(menuRect.bottom-menuRect.top-lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT));
		if (lpItems->OffY < menuRect.bottom-menuRect.top-lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT)) { lpItems->OffY = menuRect.bottom-menuRect.top-lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT); }
		if (lpItems->OffY > 0) { lpItems->OffY = 0; }
		InvalidateRect(Owner, NULL, false);

	} else if (ClickMode == CLK_DRAG) {
		PY = py;
		if ((!Dragging) && (abs(OldPY-py) > 10)) {
			Dragging = true;
		}
		if (Dragging) {
			lpItems->OffY = OldOffY-OldPY+py;
			if (lpItems->OffY < menuRect.bottom-menuRect.top-lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT)) { lpItems->OffY = menuRect.bottom-menuRect.top-lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT); }
			if (lpItems->OffY > 0) { lpItems->OffY = 0; }
			InvalidateRect(Owner, NULL, false);
		}
	}
}

void CMenu::OnMouseUp(int px, int py) {
	if (ClickMode == CLK_DRAG) {
		if (Dragging == false) {
			lpItems->ItemIndex = (int)((py-lpItems->OffY-menuRect.top)/getBrowserProp(IBP_ITEMHEIGHT));
			if (lpItems->ItemIndex >= lpItems->Count) {
				lpItems->ItemIndex = -1;
			}
			MsgProc(WBR_MENUSELECTED);
		}
	}
	ClickMode = CLK_NONE;
	InvalidateRect(Owner, NULL, false);
}

void CMenu::Constrain() {
	
	if (lpItems->ItemIndex >= 0) {
		if (lpItems->OffY < -lpItems->ItemIndex*getBrowserProp(IBP_ITEMHEIGHT)+10) {
			lpItems->OffY = -lpItems->ItemIndex*getBrowserProp(IBP_ITEMHEIGHT)+10;
		}

		if (lpItems->OffY > -(lpItems->ItemIndex+1)*getBrowserProp(IBP_ITEMHEIGHT)+menuRect.bottom-menuRect.top-10) {
			lpItems->OffY = -(lpItems->ItemIndex+1)*getBrowserProp(IBP_ITEMHEIGHT)+menuRect.bottom-menuRect.top-10;
		}
	}

	if (lpItems->OffY < -lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT)+menuRect.bottom-menuRect.top) {
		lpItems->OffY = -lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT)+menuRect.bottom-menuRect.top;
	}

	if (lpItems->OffY > 0) {
		lpItems->OffY = 0;
	}

}

void CMenu::OnPaint(HDC hDC) {
	
	HBRUSH bgBrush = CreateSolidBrush(getBrowserColor(IBC_ITEMSBG));
	HBRUSH bgSelBrush = CreateSolidBrush(getBrowserColor(IBC_SELITEMBG));
	HBRUSH bgCheckBrush = CreateSolidBrush(getBrowserColor(IBC_CHECKED));

	bool Scroller = (menuRect.bottom-menuRect.top < lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT));
	if (menuBG.Exists == false) {
		menuBG.Exists = true;
		BitBlt(menuBG.hDC, 0, 0, get_devWid(), get_devHei(), hDC, 0, 0, SRCCOPY);
		LPRECT menuBrdRect = getRect(get_devWid()/20-2, get_devHei()/20-2, get_devWid()*19/20+2, get_devHei()*19/20+2);
		FillRect(menuBG.hDC, menuBrdRect, bgSelBrush);
		delete menuBrdRect;
		FillRect(menuBG.hDC, &menuRect, bgBrush);		
	}

	HBITMAP hBufBmp = CreateBitmap(menuRect.right-menuRect.left, menuRect.bottom-menuRect.top, 1, 16, NULL);
	HDC hBufDC = CreateCompatibleDC(NULL);
	HBITMAP hBufBmpOld = (HBITMAP)SelectObject(hBufDC, hBufBmp);
	
	LPRECT menuClientRect = getRect(0, 0, menuRect.right-menuRect.left-Scroller*getBrowserProp(IBP_SCROLLERSIZE), menuRect.bottom-menuRect.top);
	FillRect(hBufDC, menuClientRect, bgBrush);

	if (Scroller) {
		HBRUSH bgScrollerBrush = CreateSolidBrush(getBrowserColor(IBC_SCROLLERBG));
		HBRUSH fgScrollerBrushL = CreateSolidBrush(MixColors(getBrowserColor(IBC_SCROLLER), RGB(255, 255, 255), 0.05+0.15*(ClickMode == CLK_SCROLLER)));
		HBRUSH fgScrollerBrushS = CreateSolidBrush(MixColors(MixColors(getBrowserColor(IBC_SCROLLER), RGB(0, 0, 0), 0.05), RGB(255,255,255), 0.12*(ClickMode == CLK_SCROLLER)));

		LPRECT scrollerClientRect = getRect(menuRect.right-menuRect.left-getBrowserProp(IBP_SCROLLERSIZE), 0, menuRect.right, menuRect.bottom-menuRect.top);
		FillRect(hBufDC, scrollerClientRect, bgScrollerBrush);

		HPEN lnScr = CreatePen(PS_SOLID, 1, getBrowserColor(IBC_LINE));
		SelectObject(hBufDC, (HPEN)lnScr);
		MoveToEx(hBufDC, scrollerClientRect->left, 0, NULL);
		LineTo(hBufDC, scrollerClientRect->left, scrollerClientRect->bottom);
		DeleteObject(lnScr);

		int scHei = max(getBrowserProp(IBP_SCROLLERSIZE)/2, (menuRect.bottom-menuRect.top)*(menuRect.bottom-menuRect.top)/(lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT)));
		int scTop = (int)((menuRect.bottom-menuRect.top-scHei)*((double)lpItems->OffY/(menuRect.bottom-menuRect.top-lpItems->Count*getBrowserProp(IBP_ITEMHEIGHT))));

		LPRECT scrollerRect = getRect(menuRect.right-menuRect.left-getBrowserProp(IBP_SCROLLERSIZE)+1, scTop, menuRect.right-menuRect.left-getBrowserProp(IBP_SCROLLERSIZE)/2, scTop+scHei);
		FillRect(hBufDC, scrollerRect, fgScrollerBrushL);
		delete scrollerRect;

		scrollerRect = getRect(menuRect.right-menuRect.left-getBrowserProp(IBP_SCROLLERSIZE)/2, scTop, menuRect.right-menuRect.left, scTop+scHei);
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
		DeleteObject(bgScrollerBrush);
	}

	HBRUSH bgRightSideBrush = CreateSolidBrush(getBrowserColor(IBC_RIGHTSIDE));
	LPRECT iconRect = getRect(0, 0, getBrowserProp(IBP_ITEMHEIGHT), menuClientRect->bottom);
	FillRect(hBufDC, iconRect, bgRightSideBrush);
	delete iconRect;
	DeleteObject(bgRightSideBrush);

	HPEN lnPen = CreatePen(PS_DASH, 1, getBrowserColor(IBC_LINE));
	HPEN lnSep = CreatePen(PS_SOLID, 1, getBrowserColor(IBC_LINE));
	int wei = FW_NORMAL;
	if (getBrowserProp(IBP_ITEMFONTBOLD) == 1) {
		wei = FW_BOLD;
	}
	HFONT fnt = CreateFont(getBrowserProp(IBP_ITEMFONTSIZE), wei, false);
	
	SetTextColor(hBufDC, getBrowserColor(IBC_ITEMSTEXT));
	SetBkMode(hBufDC, TRANSPARENT);
	SelectObject(hBufDC, fnt);

	for (int i = 0; i < lpItems->Count; i++) {
		int py = lpItems->OffY+i*getBrowserProp(IBP_ITEMHEIGHT);
		if (py > menuClientRect->bottom) {
			break;
		} else if (py+getBrowserProp(IBP_ITEMHEIGHT) >= 0) {
			if (i == lpItems->ItemIndex) {
				int inc = 0;
				if (i > 0) {
					inc = 1;
				}
				LPRECT rct = getRect(0, py+inc, menuClientRect->right, py+getBrowserProp(IBP_ITEMHEIGHT));
				FillRect(hBufDC, rct, bgSelBrush);
				SetTextColor(hBufDC, getBrowserColor(IBC_SELITEMTEXT));
				delete rct;
			}
			if (lpItems->Data[i].Checked) {
				LPRECT rcc = getRect(2, py+2, getBrowserProp(IBP_ITEMHEIGHT)-2, py+getBrowserProp(IBP_ITEMHEIGHT)-2);
				FillRect(hBufDC, rcc, bgCheckBrush);
			}
			LPRECT rct = getRect(getBrowserProp(IBP_ITEMHEIGHT)+4, py, menuClientRect->right-5, py+getBrowserProp(IBP_ITEMHEIGHT));
			if ((getIcons()->Count > lpItems->Data[i].IconIndex) && (getIcon(lpItems->Data[i].IconIndex)->Exists)) {
				TransparentBlt(hBufDC, (getBrowserProp(IBP_ITEMHEIGHT)-getIcons()->IconSize)/2, py+(getBrowserProp(IBP_ITEMHEIGHT)-getIcons()->IconSize)/2, getIcons()->IconSize, getIcons()->IconSize, getIcon(lpItems->Data[i].IconIndex)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
			}
			if ((lpItems->Data[i].Arrow) && (getIcons()->Count > ICO_ARROW) && (getIcon(ICO_ARROW)->Exists)) {
				TransparentBlt(hBufDC, menuClientRect->right-getIcons()->IconSize-4, py+(getBrowserProp(IBP_ITEMHEIGHT)-getIcons()->IconSize)/2, getIcons()->IconSize, getIcons()->IconSize, getIcon(ICO_ARROW)->hDC, 0, 0, getIcons()->IconSize, getIcons()->IconSize, RGB(255, 0, 255));
				rct->right = menuClientRect->right-getIcons()->IconSize-6;
			}
			DrawText(hBufDC, lpItems->Data[i].Text, lpItems->Data[i].TextLength, rct, DT_VCENTER|DT_LEFT|DT_WORD_ELLIPSIS|DT_NOPREFIX);
			if (lpItems->Data[i].Separator) {
				SelectObject(hBufDC, (HPEN)lnSep);
			} else {
				SelectObject(hBufDC, (HPEN)lnPen);
			}
			MoveToEx(hBufDC, 0, py+getBrowserProp(IBP_ITEMHEIGHT), NULL);
			LineTo(hBufDC, menuClientRect->right, py+getBrowserProp(IBP_ITEMHEIGHT));
			if (i == lpItems->ItemIndex) {
				SetTextColor(hBufDC, getBrowserColor(IBC_ITEMSTEXT));
			}
			delete rct;
		}
	}

	BitBlt(menuBG.hDC, menuRect.left, menuRect.top, menuRect.right-menuRect.left, menuRect.bottom-menuRect.top, hBufDC, 0, 0, SRCCOPY);
	BitBlt(hDC, 0, 0, get_devWid(), get_devHei(), menuBG.hDC, 0, 0, SRCCOPY);
	DeleteObject(bgBrush);
	DeleteObject(bgSelBrush);
	DeleteObject(lnPen);
	DeleteObject(lnSep);
	DeleteObject(fnt);
	DeleteObject(bgCheckBrush);

	SelectObject(hBufDC, hBufBmpOld);
	DeleteObject(hBufBmp);
	DeleteDC(hBufDC);
	delete menuClientRect;

}


void CMenu::Start(WBRLIST * pItems) {
	UpdateMenuSize();
	lpItems = pItems;
	DeleteXBitmap(&menuBG);
	CreateXBitmap(&menuBG, get_devWid(), get_devHei(), 16, false);
}

CMenu::~CMenu() {

	DeleteXBitmap(&menuBG);

}
