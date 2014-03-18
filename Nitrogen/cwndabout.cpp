#include "stdafx.h"
#include "main.h"

CWndAbout::CWndAbout() {
	bg.Exists = false;
}

ATOM CWndAbout::InitClass() {
	WNDCLASS wc;
	wc.style  = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = CWndAboutProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = player()->hInstance;
	wc.hIcon = LoadIcon(player()->hInstance, MAKEINTRESOURCE(ICO_MAIN));
	wc.hCursor = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"CNtgWndAbout";
	return RegisterClass(&wc);
}

bool CWndAbout::Show() {
	
	HWND hWnd = CreateWindow(L"CNtgWndAbout", _str(STR_ABOUT), WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, player()->hInstance, NULL);
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

void CWndAbout::OnCreate() {

	LPWSTR abText[8] = {L"Copyright © - Filipe Vilela Soares",
					L"http://microfi-nitrogen.googlecode.com",
					_str(STR_PLEASEDONATEIFYO),
					_str(STR_MANYTHANKSTOEVER),
					_str(STR_HELPEDMAKETHISPR),
					_str(STR_STABLEANDGOODLOO),
					_str(STR_LANGUAGETRANSLAT),
					_str(STR_BYAUTHOR)};

#ifdef SHELL_AYGSHELL
	// Creates menu bar
    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(SHMENUBARINFO));
    mbi.cbSize = sizeof(SHMENUBARINFO);
    mbi.hwndParent = hWnd;
    mbi.nToolBarId = MNU_OKCANCEL;
    mbi.hInstRes = player()->hInstance;
    if (!SHCreateMenuBar(&mbi)) {
        hWndMenuBar = NULL;
    } else {
        hWndMenuBar = mbi.hwndMB;
		SetSoftkeyText(hWndMenuBar, 0, _str(STR_CANCEL));
		SetSoftkeyText(hWndMenuBar, 1, _str(STR_OK));
	}
#else
    hWndMenuBar = NULL;
#endif

	// Draws on the buffer image
	CreateXBitmap(&bg, get_devWid(), get_devHei(), 16, true);

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = get_devWid();
	rc.bottom = get_devHei();
	FillRect(bg.hDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

	rc.right = get_devWid()/4;
	HBRUSH newBrush = CreateSolidBrush(RGB(210, 230, 250));
	FillRect(bg.hDC, &rc, newBrush);

	int rad = get_devWid()/4;
	int dx = (int) ((double)rad * sin(PI/3));
	int dy = (int) ((double)rad * cos(PI/3));

	SelectObject(bg.hDC, GetStockObject(NULL_PEN));

	HBRUSH oldBrush = (HBRUSH)SelectObject(bg.hDC, newBrush);
	int y = 0;

	while (y < get_devHei()+rad) {
		
		Ellipse(bg.hDC, rc.right-dx-rad, y-rad, rc.right-dx+rad, y+rad);
		y += dy*4;

	}

	SelectObject(bg.hDC, oldBrush);
	y = dy*2;

	while (y < get_devHei()+rad) {
		
		Ellipse(bg.hDC, rc.right+dx-rad, y-rad, rc.right+dx+rad, y+rad);
		y += dy*4;

	}
	DeleteObject(newBrush);

	TEXTMETRIC metrics;
	GetTextMetrics(bg.hDC, &metrics);
	HFONT titleFont = CreateFont(1.5*metrics.tmHeight, FW_BOLD, false);

	HFONT oldFont = (HFONT)SelectObject(bg.hDC, (HFONT)titleFont);
	rc.right = get_devWid();
	rc.bottom = metrics.tmHeight*2;
	SetBkMode(bg.hDC, TRANSPARENT);
	DrawText(bg.hDC, L"Microfi Nitrogen 1.1", 20, &rc, DT_CENTER|DT_BOTTOM);

	HFONT textFont = CreateFont(metrics.tmHeight, FW_NORMAL, false);
	SelectObject(bg.hDC, (HFONT)textFont);

	int spc = metrics.tmHeight*10;
	int top = metrics.tmHeight*2+(get_devHei()-metrics.tmHeight*2-spc)/2;
	
	for (int i = 0; i < 8; i++) {
		
		rc.top = top;
		rc.bottom = top+metrics.tmHeight;

		DrawText(bg.hDC, abText[i], wcslen(abText[i]), &rc, DT_CENTER|DT_VCENTER);
		top += metrics.tmHeight;
		if ((i == 0) | (i == 2) | (i == 5)) {
			top += metrics.tmHeight/2;
		}
	}

	SelectObject(bg.hDC, oldFont);
	DeleteObject(titleFont);
	DeleteObject(textFont);

}

void CWndAbout::OnPaint(HDC hDC) {
	
	BitBlt(hDC, 0, 0, get_devWid(), get_devHei(), bg.hDC, 0, 0, SRCCOPY);

}

LRESULT CWndAbout::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	
		case WM_CREATE:
			OnCreate();
			break;

		case WM_ERASEBKGND:
			return 0;

		case WM_PAINT: {
			HDC hDC;
			PAINTSTRUCT ps;
			hDC = BeginPaint(hWnd, &ps);
			OnPaint(hDC);
            EndPaint(hWnd, &ps);
            break;
		}
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			if ((wmId == IDM_OK) | (wmId == IDM_CANCEL) | (wmId == IDOK)) {
				DestroyWindow(hWnd);
			}
			break;
		}

		case WM_ACTIVATE:
			if ((wParam == WA_INACTIVE) & ((HWND)lParam == player()->lpWndMain->hWnd)) {
				DestroyWindow(hWnd);
			} else {
				DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			break;

		case WM_DESTROY:
			CommandBar_Destroy(hWndMenuBar);
			DeleteXBitmap(&bg);
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK CWndAboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (uMsg == WM_CREATE) {
		player()->lpWndAbout->hWnd = hWnd;
	}
	return player()->lpWndAbout->WndProc(uMsg, wParam, lParam);
}

CWndAbout::~CWndAbout() {
	DeleteXBitmap(&bg);
}