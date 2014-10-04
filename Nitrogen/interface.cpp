// interface.cpp - Skin support for Nitrogen

#include "stdafx.h"
#include "main.h"

int devWid, devHei;

XBITMAP Background, ModBG;
IITEMS Items;
IBROWSER Browser;
IICONS Icons;
wchar_t lpAlbumArt[MAX_PATH];

WTEXTSTR Texts[20];
double Trackbars[10];
bool CmdToggle[40];

int get_devWid() {
	return devWid;
}

int get_devHei() {
	return devHei;
}

IITEM * getItem(int Index) {
	return &Items.Data[Index];
}

int getItemsCount() {
	return Items.Count;
}

COLORREF getBrowserColor(int Index) {
	if (Index == IBC_SCROLLERBG) {
		return Browser.Color[IBC_TITLEBG];
	} else {
		return (COLORREF)Browser.Color[Index];
	}
}

int getBrowserProp(int Index) {
	if (Index == IBP_SCROLLERSIZE) {
		return (devWid+devHei)/30;
	} else {
		return Browser.IntProp[Index];
	}
}

IICONS * getIcons() {
	return &Icons;
}

XBITMAP * getIcon(int Index) {
	return &Icons.Data[Index];
}

void StartInterface() {
	for (int i = 0; i < 20; i++) {
		Texts[i][0] = 0;
	}

	for (int i = 0; i < 10; i++) {
		Trackbars[i] = 0;
	}

	for (int i = 0; i < 40; i++) {
		CmdToggle[i] = false;
	}

	Background.Exists = false;
	Background.hBitmap = NULL;
	Background.hDC = NULL;
	
	ModBG.Exists = false;
	ModBG.hBitmap = NULL;
	ModBG.hDC = NULL;
	Items.Data = NULL;
	Browser.Color = NULL;
	Browser.IntProp = NULL;
}

bool iLoadSkin(HWND hWnd, LPWSTR fileName, bool quiet) {
	
	HANDLE f = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (f != INVALID_HANDLE_VALUE) {
		
		int ibuf = 0;
		DWORD n;
		ReadFile(f, &ibuf, 4, &n, false);
		if (ibuf != 2000) {
			if (!quiet) {
				MessageBox(hWnd, _str(STR_THEVERSIONOFTHES), _str(STR_ERROR), MB_OK|MB_ICONERROR);
			}
			CloseHandle(f);	
			return false;
		}
		
		DeleteXBitmap(&Background);
		ReadFileBitmap(f, &Background);
		devWid = Background.Width;
		devHei = Background.Height;

		memset(lpAlbumArt, 0, sizeof(lpAlbumArt));

		DeleteXBitmap(&ModBG);
		ModBG.Exists = true;
		ModBG.hBitmap = CreateBitmap(devWid, devHei, 1, 16, NULL);
		ModBG.hDC = CreateCompatibleDC(NULL);
		ModBG.hBitmapOld = (HBITMAP)SelectObject(ModBG.hDC, ModBG.hBitmap);
		
		BitBlt(ModBG.hDC, 0, 0, devWid, devHei, Background.hDC, 0, 0, SRCCOPY);

		if (Items.Data != NULL) {
			for (int i = 0; i < Items.Count; i++) {
				for (int x = 0; x < Items.Data[i].BmpPropCount; x++) {
					DeleteXBitmap(&Items.Data[i].BmpProp[x]);
				}
				delete [] Items.Data[i].BmpProp;
				delete [] Items.Data[i].IntProp;
			}
			delete [] Items.Data;
		}

		Items.Count = 0;
		ReadFile(f, &Items.Count, 2, &n, false);
		Items.Data = new IITEM[Items.Count];
		int textitem_num = 0;
		for (int i = 0; i < Items.Count; i++) {
			Items.Data[i].Kind = (IITEMKIND)0;
			ReadFile(f, &Items.Data[i].Kind, 2, &n, false);
		
			Items.Data[i].BmpPropCount = 0;
			ReadFile(f, &Items.Data[i].BmpPropCount, 2, &n, false);
			Items.Data[i].BmpProp = new XBITMAP[Items.Data[i].BmpPropCount];
			for (int x = 0; x < Items.Data[i].BmpPropCount; x++) {
				ReadFileBitmap(f, &Items.Data[i].BmpProp[x]);
			}

			Items.Data[i].IntPropCount = 0;
			ReadFile(f, &Items.Data[i].IntPropCount, 2, &n, false);
			Items.Data[i].IntProp = new int[Items.Data[i].IntPropCount];
			for (int x = 0; x < Items.Data[i].IntPropCount; x++) {
				ReadFile(f, &Items.Data[i].IntProp[x], 4, &n, false);
			}
			if (Items.Data[i].Kind == IIK_TEXT) {
				if (textitem_num ==0) {
					Items.Data[i].IntProp[ITP_TEXTTYPE]=ITT_TRACKNUMBER;
				}
				if (textitem_num ==1) {
					Items.Data[i].IntProp[ITP_TEXTTYPE]=ITT_TRACKCOUNT;
				}
				textitem_num++;
			}
			Items.Data[i].CustomProp = 0;
			Items.Data[i].CustomProp1 = 0;
		}

		Browser.ColorCount = 0;
		ReadFile(f, &Browser.ColorCount, 2, &n, false);
		delete [] Browser.Color;
		Browser.Color = new int[Browser.ColorCount];
		for (int i = 0; i < Browser.ColorCount; i++) {
			ReadFile(f, &Browser.Color[i], 4, &n, false);
		}
		Browser.IntPropCount = 0;
		ReadFile(f, &Browser.IntPropCount, 2, &n, false);
		delete [] Browser.IntProp;
		Browser.IntProp = new int[Browser.IntPropCount];
		for (int i = 0; i < Browser.IntPropCount; i++) {
			ReadFile(f, &Browser.IntProp[i], 4, &n, false);
		}

		if (Icons.Data != NULL) {
			for (int i = 0; i < Icons.Count; i++) {
				DeleteXBitmap(&Icons.Data[i]);
			}
			delete [] Icons.Data;
		}

		Icons.IconSize = 0;
		ReadFile(f, &Icons.IconSize, 2, &n, false);
		Icons.Count = 0;
		ReadFile(f, &Icons.Count, 2, &n, false);
		Icons.Data = new XBITMAP[Icons.Count];
		for (int i = 0; i < Icons.Count; i++) {
			ReadFileBitmap(f, &Icons.Data[i]);
		}
		CloseHandle(f);

	} else {
		if (!quiet) {
			MessageBox(hWnd, _str(STR_THESKINFILEWASNO), _str(STR_ERROR), MB_OK|MB_ICONERROR);
		}
		return false; 
	}
	return true;
}

bool RectsIntersect(LPRECT l1, LPRECT l2) {
	
	return ((l1->left >= l2->left-(l1->right-l1->left)) && (l1->top >= l2->top-(l1->bottom-l1->top)) && (l1->left <= l2->right) && (l1->top <= l2->bottom));
}

void iDrawTexts(HDC hDC, LPRECT area, int alsoType) {

	for (int i = 0; i < Items.Count; i++) {
	
		if (Items.Data[i].Kind == IIK_TEXT) {

			if (Items.Data[i].IntProp[ITP_TEXTTYPE] == alsoType) {
				iDrawTextBg(hDC, i, 0, 0);
			} else if (area != NULL) {
				LPRECT trect = getRect(Items.Data[i].IntProp[ITP_XPOS], Items.Data[i].IntProp[ITP_YPOS], Items.Data[i].IntProp[ITP_XPOS]+Items.Data[i].IntProp[ITP_WIDTH], Items.Data[i].IntProp[ITP_YPOS]+Items.Data[i].IntProp[ITP_HEIGHT]);
				if (RectsIntersect(area, trect)) {
					iDrawTextBg(hDC, i, 0, 0);
				}
				delete trect;
			}
			
		}
	}
}

void iDrawAlbumArt(HDC hDC) {

	if (hDC == 0) {

		if (player()->lpPlaylist->IndexAlbumArtFilename[0] != 0) {
			if (strCompare(lpAlbumArt, player()->lpPlaylist->IndexAlbumArtFilename) != 0) {
				for (int i = 0; i < Items.Count; i++) {
					if ((Items.Data[i].Kind == IIK_SPECIALAREA) && (Items.Data[i].IntProp[ISP_AREATYPE] == IST_ALBUMART)) {
						XBITMAP xbit;
						if (LoadJPEG(player()->lpPlaylist->IndexAlbumArtFilename, &xbit, Items.Data[i].IntProp[ISP_WIDTH], Items.Data[i].IntProp[ISP_HEIGHT])) {
							BitBlt(ModBG.hDC, Items.Data[i].IntProp[ISP_XPOS], Items.Data[i].IntProp[ISP_YPOS], Items.Data[i].IntProp[ISP_WIDTH], Items.Data[i].IntProp[ISP_HEIGHT], xbit.hDC, 0, 0, SRCCOPY);
							DeleteXBitmap(&xbit);
						} else {
							BitBlt(ModBG.hDC, Items.Data[i].IntProp[ISP_XPOS], Items.Data[i].IntProp[ISP_YPOS], Items.Data[i].IntProp[ISP_WIDTH], Items.Data[i].IntProp[ISP_HEIGHT], Background.hDC, Items.Data[i].IntProp[ISP_XPOS], Items.Data[i].IntProp[ISP_YPOS], SRCCOPY);
						}
					}
				}
				wcscpy(lpAlbumArt, player()->lpPlaylist->IndexAlbumArtFilename);
			}

		} else {
			if (lpAlbumArt[0] != 0) {
				lpAlbumArt[0] = 0;
				for (int i = 0; i < Items.Count; i++) {
					if ((Items.Data[i].Kind == IIK_SPECIALAREA) && (Items.Data[i].IntProp[ISP_AREATYPE] == IST_ALBUMART)) {
						BitBlt(ModBG.hDC, Items.Data[i].IntProp[ISP_XPOS], Items.Data[i].IntProp[ISP_YPOS], Items.Data[i].IntProp[ISP_WIDTH], Items.Data[i].IntProp[ISP_HEIGHT], Background.hDC, Items.Data[i].IntProp[ISP_XPOS], Items.Data[i].IntProp[ISP_YPOS], SRCCOPY);
					}
				}
			}
		}

	} else {
		for (int i = 0; i < Items.Count; i++) {
			if ((Items.Data[i].Kind == IIK_SPECIALAREA) && (Items.Data[i].IntProp[ISP_AREATYPE] == IST_ALBUMART)) {
				BitBlt(hDC, Items.Data[i].IntProp[ISP_XPOS], Items.Data[i].IntProp[ISP_YPOS], Items.Data[i].IntProp[ISP_WIDTH], Items.Data[i].IntProp[ISP_HEIGHT], ModBG.hDC, Items.Data[i].IntProp[ISP_XPOS], Items.Data[i].IntProp[ISP_YPOS], SRCCOPY);
			}	
		}
	}

}

void iSetText(ITEXTTYPE tType, LPWSTR newText) {
	wcscpy(Texts[tType], newText);
}

void iSetCmdToggle(ICOMMAND Cmd, bool newToggle) {
	CmdToggle[Cmd] = newToggle;
}

void iDrawText(HDC hDC, int Index, int offX, int offY) {
	LPRECT rct = getRect(offX+Items.Data[Index].IntProp[ITP_XPOS], offY+Items.Data[Index].IntProp[ITP_YPOS], offX+Items.Data[Index].IntProp[ITP_XPOS]+Items.Data[Index].IntProp[ITP_WIDTH], offY+Items.Data[Index].IntProp[ITP_YPOS]+Items.Data[Index].IntProp[ITP_HEIGHT]);
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, Items.Data[Index].IntProp[ITP_FONTCOLOR]);
	int wei = FW_NORMAL;
	if (Items.Data[Index].IntProp[ITP_FONTBOLD] == 1) {
		wei = FW_BOLD;
	}
	HFONT fnt = CreateFont(Items.Data[Index].IntProp[ITP_FONTSIZE], wei, false);
	int opt = DT_WORD_ELLIPSIS|DT_NOPREFIX;
	switch (Items.Data[Index].IntProp[ITP_HORZALIGN]) {
		case 0: {
			opt = opt|DT_LEFT;
			break;
		}
		case 1: {
			opt = opt|DT_CENTER;
			break;
		}
		case 2: {
			opt = opt|DT_RIGHT;
			break;
		}
	}
	switch (Items.Data[Index].IntProp[ITP_VERTALIGN]) {
		case 0: {
			opt = opt|DT_TOP;
			break;
		}
		case 1: {
			opt = opt|DT_VCENTER;
			break;
		}
		case 2: {
			opt = opt|DT_BOTTOM;
			break;
		}
	}
	SelectObject(hDC, fnt);
	DrawText(hDC, Texts[Items.Data[Index].IntProp[ITP_TEXTTYPE]], wcslen(Texts[Items.Data[Index].IntProp[ITP_TEXTTYPE]]), rct, opt);
	delete rct;
	SetBkMode(hDC, OPAQUE);
}

void iDrawTextBg(HDC hDC, int Index, int offX, int offY) {
	HBITMAP hBufBmp = CreateBitmap(Items.Data[Index].IntProp[ITP_WIDTH], Items.Data[Index].IntProp[ITP_HEIGHT], 1, 16, NULL);
	HDC hBufDC = CreateCompatibleDC(NULL);
	HBITMAP hBufBmpOld = (HBITMAP)SelectObject(hBufDC, hBufBmp);
	BitBlt(hBufDC, 0, 0, Items.Data[Index].IntProp[ITP_WIDTH], Items.Data[Index].IntProp[ITP_HEIGHT], ModBG.hDC, Items.Data[Index].IntProp[ITP_XPOS], Items.Data[Index].IntProp[ITP_YPOS], SRCCOPY);
	iDrawText(hBufDC, Index, -Items.Data[Index].IntProp[ITP_XPOS], -Items.Data[Index].IntProp[ITP_YPOS]);
	BitBlt(hDC, Items.Data[Index].IntProp[ITP_XPOS], Items.Data[Index].IntProp[ITP_YPOS], Items.Data[Index].IntProp[ITP_WIDTH], Items.Data[Index].IntProp[ITP_HEIGHT], hBufDC, 0, 0, SRCCOPY);
	SelectObject(hBufDC, hBufBmpOld);
	DeleteObject(hBufBmp);
	DeleteDC(hBufDC);
}

void iDrawWheel(HDC hDC, int Index, int offX, int offY) {
	HBITMAP hBufBmp = CreateBitmap(Items.Data[Index].IntProp[IWP_RADIUS]*2, Items.Data[Index].IntProp[IWP_RADIUS]*2, 1, 16, NULL);
	HDC hBufDC = CreateCompatibleDC(NULL);
	HBITMAP hBufBmpOld = (HBITMAP)SelectObject(hBufDC, hBufBmp);
	int mod = (Items.Data[Index].CustomProp == IWM_WHEELPRESSED) ? IWB_PRESSED : IWB_NORMAL;
	BitBlt(hBufDC, 0, 0, Items.Data[Index].IntProp[IWP_RADIUS]*2, Items.Data[Index].IntProp[IWP_RADIUS]*2, Items.Data[Index].BmpProp[IWB_WHEEL+mod].hDC, 0, 0, SRCCOPY);

	for (int i = 0; i < Items.Data[Index].IntProp[IWP_BALLCNT]; i++) {
		int xpos = (int) (Items.Data[Index].IntProp[IWP_RADIUS]+Items.Data[Index].IntProp[IWP_BALLDIST]*cos(Items.Data[Index].CustomProp1+(double)i/Items.Data[Index].IntProp[IWP_BALLCNT]*2*PI)-Items.Data[Index].BmpProp[IWB_BALL].Width/2);
		int ypos = (int) (Items.Data[Index].IntProp[IWP_RADIUS]+Items.Data[Index].IntProp[IWP_BALLDIST]*sin(Items.Data[Index].CustomProp1+(double)i/Items.Data[Index].IntProp[IWP_BALLCNT]*2*PI)-Items.Data[Index].BmpProp[IWB_BALL].Height/2);
		TransparentBlt(hBufDC, xpos, ypos, Items.Data[Index].BmpProp[IWB_BALL].Width, Items.Data[Index].BmpProp[IWB_BALL].Height, Items.Data[Index].BmpProp[IWB_BALL].hDC, 0, 0, Items.Data[Index].BmpProp[IWB_BALL].Width, Items.Data[Index].BmpProp[IWB_BALL].Height, RGB(255, 0, 255));
	}
	mod = (Items.Data[Index].CustomProp == IWM_LPRESSED) ? IWB_PRESSED : IWB_NORMAL;
	int tog = (CmdToggle[Items.Data[Index].IntProp[IWP_LCOMMAND]]) ? 2 : 0;
	TransparentBlt(hBufDC, 0, Items.Data[Index].IntProp[IWP_RADIUS]-Items.Data[Index].IntProp[IWP_SIDEBTNSIZE], Items.Data[Index].BmpProp[IWB_LEFTBTN].Width, Items.Data[Index].BmpProp[IWB_LEFTBTN].Height, Items.Data[Index].BmpProp[IWB_LEFTBTN+mod+tog].hDC, 0, 0, Items.Data[Index].BmpProp[IWB_LEFTBTN].Width, Items.Data[Index].BmpProp[IWB_LEFTBTN].Height, RGB(255, 0, 255));
	
	mod = (Items.Data[Index].CustomProp == IWM_RPRESSED) ? IWB_PRESSED : IWB_NORMAL;
	tog = (CmdToggle[Items.Data[Index].IntProp[IWP_RCOMMAND]]) ? 2 : 0;
	TransparentBlt(hBufDC, Items.Data[Index].IntProp[IWP_RADIUS]*2-Items.Data[Index].BmpProp[IWB_RIGHTBTN].Width, Items.Data[Index].IntProp[IWP_RADIUS]-Items.Data[Index].IntProp[IWP_SIDEBTNSIZE], Items.Data[Index].BmpProp[IWB_RIGHTBTN].Width, Items.Data[Index].BmpProp[IWB_RIGHTBTN].Height, Items.Data[Index].BmpProp[IWB_RIGHTBTN+mod+tog].hDC, 0, 0, Items.Data[Index].BmpProp[IWB_RIGHTBTN].Width, Items.Data[Index].BmpProp[IWB_RIGHTBTN].Height, RGB(255, 0, 255));

	mod = (Items.Data[Index].CustomProp == IWM_CPRESSED) ? IWB_PRESSED : IWB_NORMAL;
	tog = (CmdToggle[Items.Data[Index].IntProp[IWP_CCOMMAND]]) ? 2 : 0;
	TransparentBlt(hBufDC, Items.Data[Index].IntProp[IWP_RADIUS]-Items.Data[Index].IntProp[IWP_CENTERRADIUS], Items.Data[Index].IntProp[IWP_RADIUS]-Items.Data[Index].IntProp[IWP_CENTERRADIUS], Items.Data[Index].BmpProp[IWB_CENTER].Width, Items.Data[Index].BmpProp[IWB_CENTER].Height, Items.Data[Index].BmpProp[IWB_CENTER+mod+tog].hDC, 0, 0, Items.Data[Index].BmpProp[IWB_CENTER].Width, Items.Data[Index].BmpProp[IWB_CENTER].Height, RGB(255, 0, 255));

	TransparentBlt(hDC, Items.Data[Index].IntProp[IWP_XCENTER]-Items.Data[Index].IntProp[IWP_RADIUS]+offX, Items.Data[Index].IntProp[IWP_YCENTER]-Items.Data[Index].IntProp[IWP_RADIUS]+offY, Items.Data[Index].IntProp[IWP_RADIUS]*2, Items.Data[Index].IntProp[IWP_RADIUS]*2, hBufDC, 0, 0, Items.Data[Index].IntProp[IWP_RADIUS]*2, Items.Data[Index].IntProp[IWP_RADIUS]*2, RGB(255, 0, 255));
	SelectObject(hBufDC, hBufBmpOld);
	DeleteObject(hBufBmp);
	DeleteDC(hBufDC);
}

void iDrawButton(HDC hDC, int Index, int offX, int offY) {
	int mod = 0;
	if (CmdToggle[Items.Data[Index].IntProp[IBP_COMMAND]]) {
		mod = 2;
	}
	if (Items.Data[Index].CustomProp == IBM_PRESSED) {
		mod++;
	}
	TransparentBlt(hDC, Items.Data[Index].IntProp[IBP_XPOS]+offX, Items.Data[Index].IntProp[IBP_YPOS]+offY, Items.Data[Index].BmpProp[0].Width, Items.Data[Index].BmpProp[0].Height, Items.Data[Index].BmpProp[mod].hDC, 0, 0, Items.Data[Index].BmpProp[0].Width, Items.Data[Index].BmpProp[0].Height, RGB(255, 0, 255));
}

void iDrawTrackbar(HDC hDC, int Index, bool offScreen) {
	int wid = (int)(Items.Data[Index].IntProp[IRP_WIDTH]*Trackbars[Items.Data[Index].IntProp[IRP_TRACKTYPE]]);
	int spX = 0;
	int spY = 0;
	int margX = 0;
	if ((Items.Data[Index].IntProp[IRP_SHOWSCROLLER] == 1) && (Items.Data[Index].BmpProp[1].Exists)) {
		margX = Items.Data[Index].BmpProp[1].Width/2+1;
	}
	if (offScreen) {
		BitBlt(ModBG.hDC, Items.Data[Index].IntProp[IRP_XPOS]-margX, Items.Data[Index].IntProp[IRP_YPOS], Items.Data[Index].IntProp[IRP_WIDTH]+margX*2, Items.Data[Index].IntProp[IRP_HEIGHT], Background.hDC, Items.Data[Index].IntProp[IRP_XPOS]-margX, Items.Data[Index].IntProp[IRP_YPOS], SRCCOPY);
		if (wid > 0) {
			BitBlt(ModBG.hDC, Items.Data[Index].IntProp[IRP_XPOS], Items.Data[Index].IntProp[IRP_YPOS], wid, Items.Data[Index].IntProp[IRP_HEIGHT], Items.Data[Index].BmpProp[0].hDC, 0, 0, SRCCOPY);
		}
		if ((Items.Data[Index].IntProp[IRP_SHOWSCROLLER] == 1) && (Items.Data[Index].BmpProp[1].Exists)) {
			spX = wid-Items.Data[Index].BmpProp[1].Width/2;
			spY = (Items.Data[Index].IntProp[IRP_HEIGHT]-Items.Data[Index].BmpProp[1].Height)/2;
			TransparentBlt(ModBG.hDC, Items.Data[Index].IntProp[IRP_XPOS]+spX, Items.Data[Index].IntProp[IRP_YPOS]+spY, Items.Data[Index].BmpProp[1].Width, Items.Data[Index].BmpProp[1].Height, Items.Data[Index].BmpProp[1].hDC, 0, 0, Items.Data[Index].BmpProp[1].Width, Items.Data[Index].BmpProp[1].Height, RGB(255, 0, 255));
		}
	} else {
		BitBlt(hDC, Items.Data[Index].IntProp[IRP_XPOS]-margX, Items.Data[Index].IntProp[IRP_YPOS], Items.Data[Index].IntProp[IRP_WIDTH]+margX*2, Items.Data[Index].IntProp[IRP_HEIGHT], ModBG.hDC, Items.Data[Index].IntProp[IRP_XPOS]-margX, Items.Data[Index].IntProp[IRP_YPOS], SRCCOPY);
	}
}

void iDrawFull(HDC hDC) {
	HBITMAP hBufBmp = CreateBitmap(devWid, devHei, 1, 16, NULL);
	HDC hBufDC = CreateCompatibleDC(NULL);
	HBITMAP hBufBmpOld = (HBITMAP)SelectObject(hBufDC, hBufBmp);

	BitBlt(hBufDC, 0, 0, devWid, devHei, ModBG.hDC, 0, 0, SRCCOPY);
	for (int i = 0; i < Items.Count; i++) {
		if (Items.Data[i].Kind == IIK_WHEEL) { iDrawWheel(hBufDC, i, 0, 0); }
		if ((Items.Data[i].Kind == IIK_BUTTON) && (CmdToggle[Items.Data[i].IntProp[IBP_COMMAND]])) { iDrawButton(hBufDC, i, 0, 0); }
	}
	for (int i = 0; i < Items.Count; i++) {
		if (Items.Data[i].Kind == IIK_TEXT) { iDrawText(hBufDC, i, 0, 0); }
	}
	BitBlt(hDC, 0, 0, devWid, devHei, hBufDC, 0, 0, SRCCOPY);
	SelectObject(hBufDC, hBufBmpOld);
	DeleteObject(hBufBmp);
	DeleteDC(hBufDC);
}

IWHEELCURPOS iWheelCurPos(int Index, int pX, int pY) {
	int wCX = Items.Data[Index].IntProp[IWP_XCENTER];
	int wCY = Items.Data[Index].IntProp[IWP_YCENTER];
	int dist = (int)sqrt((pX-wCX)*(pX-wCX)+(pY-wCY)*(pY-wCY));
	if (dist < Items.Data[Index].IntProp[IWP_CENTERRADIUS]) {
		return IWC_CBUTTON;
	} else if (dist < Items.Data[Index].IntProp[IWP_RADIUS]) {
		if ((pY > wCY-Items.Data[Index].IntProp[IWP_SIDEBTNSIZE]) && (pY < wCY+Items.Data[Index].IntProp[IWP_SIDEBTNSIZE])) {
			if (pX < wCX) {
				return IWC_LBUTTON;
			} else {
				return IWC_RBUTTON;
			}
		} else {
			return IWC_WHEEL;
		}
	} else {
		return IWC_OUTSIDE;
	}
}

IBUTTONCURPOS iButtonCurPos(int Index, int pX, int pY) {
	if ((pX < Items.Data[Index].IntProp[IBP_XPOS]) || (pY < Items.Data[Index].IntProp[IBP_YPOS]) || (pX >= Items.Data[Index].IntProp[IBP_XPOS]+Items.Data[Index].IntProp[IBP_WIDTH]) || (pY >= Items.Data[Index].IntProp[IBP_YPOS]+Items.Data[Index].IntProp[IBP_HEIGHT])) {
		return IBC_OUTSIDE;
	} else {
		bool res = (GetPixel(Items.Data[Index].BmpProp[0].hDC, pX-Items.Data[Index].IntProp[IBP_XPOS], pY-Items.Data[Index].IntProp[IBP_YPOS]) != RGB(255, 0, 255));
		return res ? IBC_INSIDE : IBC_OUTSIDE;
	}
}

ITRACKBARCURPOS iTrackbarCurPos(int Index, int pX, int pY) {
	bool res = ((pX >= Items.Data[Index].IntProp[IRP_XPOS]) && (pY >= Items.Data[Index].IntProp[IRP_YPOS]) && (pX < Items.Data[Index].IntProp[IRP_XPOS]+Items.Data[Index].IntProp[IRP_WIDTH]) && (pY < Items.Data[Index].IntProp[IRP_YPOS]+Items.Data[Index].IntProp[IRP_HEIGHT]));
	return res ? IRC_INSIDE : IRC_OUTSIDE;
}

double iGetTrackPosition(ITRACKBARTYPE tType) {
	return Trackbars[tType];
}

void iSetTrackPosition(ITRACKBARTYPE tType, double newPosition) {
	Trackbars[tType] = newPosition;
	for (int i = 0; i < Items.Count; i++) {
		if ((Items.Data[i].Kind == IIK_TRACKBAR) && (Items.Data[i].IntProp[IRP_TRACKTYPE] == tType)) {
			iDrawTrackbar(NULL, i, true);
		}
	}
}

double iCursorPosToTrackPosition(int Index, int pX) {
	double pos = (double)(pX-Items.Data[Index].IntProp[IRP_XPOS])/Items.Data[Index].IntProp[IRP_WIDTH];
	if (pos < 0) { pos = 0; }
	if (pos > 1) { pos = 1; }
	return pos;
}

void iDrawCheckbox(HDC hDC, int x, int y, int width, int height, LPWSTR text, bool checked) {
	HBRUSH boxBrush = CreateSolidBrush(RGB(255, 255, 255));
	HPEN boxPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	SelectObject(hDC, boxBrush);
	SelectObject(hDC, boxPen);

	Rectangle(hDC, x, y, x+height, y+height);
	
	DeleteObject(boxBrush);
	DeleteObject(boxPen);

	if (checked) {
		HBRUSH checkBrush = CreateSolidBrush(RGB(100, 150, 200));
		FillRect(hDC, getRect(x+3, y+3, x+height-3, y+height-3), checkBrush);
	}

	int pb = SetBkMode(hDC, TRANSPARENT);
	DrawText(hDC, text, wcslen(text), getRect(x+height+3, y, x+width, y+height), DT_VCENTER|DT_LEFT);
	SetBkMode(hDC, pb);
}

bool iPointInRect(int pX, int pY, LPRECT rct) {
	return ((pX >= rct->left) && (pY >= rct->top) && (pX < rct->right) && (pY < rct->bottom));
}

void iGetText(LPWSTR lpOut, ITEXTTYPE TextType) {
	wcscpy(lpOut, Texts[TextType]);
}
