// main.cpp - Defines the entry point for the application

#include "stdafx.h"
#include "main.h"

static CNtPlayer * g_player = NULL;

HHOOK hKBHook = NULL;
HOOKPROCS hookProcs;

CNtPlayer * player() {
	return g_player;
}

// Start point for application
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {

	// Verifies if an instance is already running
	HWND hWnd = FindWindow(L"AUDIO_WND", L"AUDIO");
    if (hWnd) {
		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
        /*
		SetForegroundWindow((HWND)((ULONG) hWnd | 0x01));
		wchar_t prCmdLine[MAX_PATH];
		if (strExtract(prCmdLine, lpCmdLine, 34)) {
			COPYDATASTRUCT cds;
			cds.dwData = 3452; // Identifies the sender as a Nitrogen instance;
			cds.cbData = (wcslen(prCmdLine)+1)*sizeof(wchar_t);
			cds.lpData = prCmdLine;
			SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
		}
		*/
		return 0;
    }

	HookLoadProcs(&hookProcs);
	if (hookProcs.SetWindowsHookEx != NULL) {
		hKBHook = hookProcs.SetWindowsHookEx(WH_KEYBOARD_LL, KBHookProc, hInstance, 0);
	}

	srand(GetTickCount());

	g_player = new CNtPlayer(hInstance, lpCmdLine);	

	player()->KeyLockSupport = (hKBHook != NULL);

	player()->lpWndMain->InitClass();
	player()->lpWndBrowser->InitClass();
	player()->lpWndEqualizer->InitClass();
	player()->lpWndAbout->InitClass();

	// Shows WndMain
	if (!g_player->lpWndMain->Show()) {
		return false;
	}

	// Main message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete g_player;
	
	if (hookProcs.UnhookWindowsHookEx != NULL && hKBHook != NULL) {
		hookProcs.UnhookWindowsHookEx(hKBHook); 
 	}
	HookCloseProcs(&hookProcs);

	return (int) msg.wParam;
}

LRESULT CALLBACK KBHookProc(int nCode, WPARAM wParam, LPARAM lParam) { 

	if (g_player->KeysLocked && nCode != VK_OFF) {
		return 1;
	}

    return hookProcs.CallNextHookEx(hKBHook, nCode, wParam, lParam); 
}

