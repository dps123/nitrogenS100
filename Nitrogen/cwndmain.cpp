#include "stdafx.h"
#include "main.h"
#include <Pm.h>

//#define _PRINT

#ifdef _PRINT
HANDLE LOG;
int FileID=0;
#endif

CWndMain::CWndMain() {

#if _WIN32_WCE >= 0x500
	hregNotifyPhone = NULL;
#endif
	lpMenu = NULL;
	inCallPlaying = false;

}

void CWndMain::CreateMenuBar(LPWSTR lText, LPWSTR rText) {
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

ATOM CWndMain::InitClass() {
	WNDCLASS wc;
	wc.style  = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)CWndMainProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = player()->hInstance;
	wc.hIcon = LoadIcon(player()->hInstance, MAKEINTRESOURCE(ICO_MAIN));
	wc.hCursor = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"AUDIO_WND";
	return RegisterClass(&wc);
}

bool CWndMain::Show() {

	HWND h = CreateWindow(L"AUDIO_WND", L"AUDIO", WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, player()->hInstance, NULL);
	if (h == NULL) {
		return false;
    }

	if (hWndMenuBar) {
        RECT rc;
        RECT rcMenuBar;
        GetWindowRect(hWnd, &rc);
        GetWindowRect(hWndMenuBar, &rcMenuBar);
		rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }
	Showing = true;
	ShowWindow(hWnd, SW_SHOW);
    Repaint(PNT_FORCEFULL, 0);
	UpdateWindow(hWnd);
	return true;	
}

void CWndMain::OnMouseDown(int px, int py) {
	
	ClickMode = CLK_NONE;

	for (int i = 0; i < getItemsCount(); i++) {
	
		switch (getItem(i)->Kind) {

			case IIK_WHEEL: {
	
				IWHEELCURPOS iWhlCP = iWheelCurPos(i, px, py);
				if (iWhlCP == IWC_WHEEL) {

					ClickMode = CLK_WHEEL;
					ClickIndex = i;
					getItem(i)->CustomProp = IWM_WHEELPRESSED;

					oldWheelRot = getItem(i)->CustomProp1; // The absolute rotation of the wheel before it starts dragging
					oldCursorRot = atan2(py-getItem(i)->IntProp[IWP_YCENTER], px-getItem(i)->IntProp[IWP_XCENTER]); // The absolute rotation of the cursor when the user presses the mouse
					
					wheelClickHalf = IWP_UPHALFAC;
					if (py > getItem(i)->IntProp[IWP_YCENTER]) {
						wheelClickHalf = IWP_LOWHALFAC;
					}

					if ((getItem(i)->IntProp[wheelClickHalf] == IWA_CHANGEMUSIC) && (player()->lpPlaylist->Count > 0)) {
						oldPlaylistIndex = player()->lpPlaylist->Index; // The playlist index before the wheel started rotating

					} else if (getItem(i)->IntProp[wheelClickHalf] == IWA_CHANGEVOLUME) {
						oldVol = player()->lpConfig->cf.Volume; // Stores the volume before the wheel started rotating

					} else if (getItem(i)->IntProp[wheelClickHalf] == IWA_SEEK && player()->PlayingSongDuration > 0) {
						oldPos = player()->PlayingSongPos; // Stores the position before the wheel started rotating
						KillTimer(hWnd, TMR_UPDATEPOS);
					}

					lastCursorRot = oldCursorRot; // The previous cursor rotation (to recognize when the user performs a turn)
					wheelTurns = 0; // The amount of entire turns (360°) the wheel has performed
					deltaRot = 0; // The variation of the rotation
					indRepaint = 0; // this prevents the wheel from repainting every little change (less waste of CPU)
					Repaint(PNT_WHEEL, 0);
					return;

				} else if ((iWhlCP == IWC_CBUTTON) || (iWhlCP == IWC_LBUTTON) || (iWhlCP == IWC_RBUTTON)) {

					ClickMode = CLK_WHEELBTN;
					ClickIndex = i;
					ClickProp = iWhlCP;
					if (iWhlCP == IWC_CBUTTON) { 
						StartSecFncTimer(getItem(i)->IntProp[IWP_CCOMMAND]);
						getItem(i)->CustomProp = IWM_CPRESSED;
					} else if (iWhlCP == IWC_LBUTTON) {
						StartSecFncTimer(getItem(i)->IntProp[IWP_LCOMMAND]);
						getItem(i)->CustomProp = IWM_LPRESSED;
					} else if (iWhlCP == IWC_RBUTTON) { 
						StartSecFncTimer(getItem(i)->IntProp[IWP_RCOMMAND]);
						getItem(i)->CustomProp = IWM_RPRESSED;
					}
					Repaint(PNT_WHEEL, 0);
					return;
				}
				break;
			}

			case IIK_BUTTON: {

				IBUTTONCURPOS iBtnCP = iButtonCurPos(i, px, py);
				if (iBtnCP == IBC_INSIDE) {
					ClickMode = CLK_BUTTON;
					ClickIndex = i;
					getItem(i)->CustomProp = IBM_PRESSED;
					Repaint(PNT_BUTTON, 0);
					StartSecFncTimer(getItem(i)->IntProp[IBP_COMMAND]);
					return;
				}
				break;

			}

			case IIK_TRACKBAR: {

				ITRACKBARCURPOS iTrkCP = iTrackbarCurPos(i, px, py);
				if (iTrkCP == IRC_INSIDE) {
					if (getItem(i)->IntProp[IRP_TRACKTYPE] == IRT_TIME && player()->PlayingSongDuration > 0) {

						ClickMode = CLK_TRACK;
						ClickIndex = i;
						KillTimer(hWnd, TMR_UPDATEPOS);
						return;

					} else if (getItem(i)->IntProp[IRP_TRACKTYPE] == IRT_VOLUME) {
					
						ClickMode = CLK_TRACK;
						ClickIndex = i;
						return;
					
					}
				}
				break;
			}
		}
	}
}

void CWndMain::OnMouseMove(int px, int py) {

	switch (ClickMode) {

		case CLK_WHEEL: {

			double cursorRot = atan2(py-getItem(ClickIndex)->IntProp[IWP_YCENTER], px-getItem(ClickIndex)->IntProp[IWP_XCENTER]); // The absolute rotation of the cursor
			
			if (cursorRot-lastCursorRot > 5.7) { // 5.7 is a number a little smaller than 2PI (6.28) - when the difference is such big, it is that the user performed a turn
				wheelTurns--;
			} else if (cursorRot-lastCursorRot < -5.7) { // That's when the user performs a turn backwards
				wheelTurns++;
			}

			lastCursorRot = cursorRot; // Updates the previous position of the cursor
			deltaRot = cursorRot-oldCursorRot+wheelTurns*2*PI; // deltaRot stores the variation of the rotation value on the wheel
			
			if ((getItem(ClickIndex)->IntProp[wheelClickHalf] == IWA_CHANGEMUSIC) && (player()->lpPlaylist->Count > 0)) {
				int newPlaylistIndex = oldPlaylistIndex+(int)(deltaRot/0.6); //Each 0.6 rad, the playlist index increments or decrements one unit
				while (newPlaylistIndex >= player()->lpPlaylist->Count) { // Constrains the playlist index
					newPlaylistIndex -= player()->lpPlaylist->Count;
				}
				while (newPlaylistIndex < 0) { // Constrains the playlist index
					newPlaylistIndex += player()->lpPlaylist->Count;
				}
				
				if (newPlaylistIndex != player()->lpPlaylist->Index) { // Sends title repaint command only if there is a change
					player()->ChangePlaylistIndex(newPlaylistIndex, false);
				}

			} else if (getItem(ClickIndex)->IntProp[wheelClickHalf] == IWA_CHANGEVOLUME) {
				int newVol = (int)(oldVol+deltaRot/10*100);
				if (newVol < 0) { newVol = 0; }
				if (newVol > 100) { newVol = 100; }
				if (newVol != player()->lpConfig->cf.Volume) {
					player()->ChangeVolume(newVol, false);
				}

			} else if ((getItem(ClickIndex)->IntProp[wheelClickHalf] == IWA_SEEK) && (player()->PlayingSongDuration > 0)) {
					
				long newPos = (long)(oldPos+deltaRot*25);
				if (newPos > player()->PlayingSongDuration-1) { newPos = player()->PlayingSongDuration-1; }
				if (newPos < 0) { newPos = 0; }
				player()->PlayingSongPos = newPos;
				UpdatePlayPos(false);

			}

			getItem(ClickIndex)->CustomProp1 = oldWheelRot+deltaRot;
			indRepaint++;
			
			if (indRepaint > 3) { // Only repaints if there is a significant rotation
				indRepaint = 0;
				Repaint(PNT_WHEEL, 0);
			}
			break;
		}

		case CLK_WHEELBTN: {

			IWHEELCURPOS iWhlCP = iWheelCurPos(ClickIndex, px, py);
			
			if ((iWhlCP == ClickProp) && (getItem(ClickIndex)->CustomProp == IWM_NORMAL)) {
				
				if (iWhlCP == IWC_CBUTTON) {       getItem(ClickIndex)->CustomProp = IWM_CPRESSED; }
				else if (iWhlCP == IWC_LBUTTON) {  getItem(ClickIndex)->CustomProp = IWM_LPRESSED; }
				else if (iWhlCP == IWC_RBUTTON) {  getItem(ClickIndex)->CustomProp = IWM_RPRESSED; }
			
				StartSecFncTimer(PREV_CMD);
				Repaint(PNT_WHEEL, 0);
			}
		
			if ((iWhlCP != ClickProp) && (getItem(ClickIndex)->CustomProp != IWM_NORMAL)) {
			
				getItem(ClickIndex)->CustomProp = IWM_NORMAL;
				KillTimer(hWnd, TMR_SECFUNCTION);
				Repaint(PNT_WHEEL, 0);
			}
			break;
		}

		case CLK_BUTTON: {

			IBUTTONCURPOS iBtnCP = iButtonCurPos(ClickIndex, px, py);
			if ((iBtnCP == IBC_INSIDE) && (getItem(ClickIndex)->CustomProp == IBM_NORMAL)) {
				getItem(ClickIndex)->CustomProp = IBM_PRESSED;
				StartSecFncTimer(PREV_CMD);
				Repaint(PNT_BUTTON, 0);
			}
	
			if ((iBtnCP == IBC_OUTSIDE) && (getItem(ClickIndex)->CustomProp == IBM_PRESSED)) {
				getItem(ClickIndex)->CustomProp = IBM_NORMAL;
				KillTimer(hWnd, TMR_SECFUNCTION);
				Repaint(PNT_BUTTON, 0);
			}
			break;
		}

		case CLK_TRACK: {
		
			double newPos = iCursorPosToTrackPosition(ClickIndex, px);
			if (getItem(ClickIndex)->IntProp[IRP_TRACKTYPE] == IRT_TIME && player()->PlayingSongDuration > 0) {
			
				int newPlayPos = (int)(newPos*player()->PlayingSongDuration);
				player()->PlayingSongPos = newPlayPos;
				UpdatePlayPos(false);
			
			} else if (getItem(ClickIndex)->IntProp[IRP_TRACKTYPE] == IRT_VOLUME) {
	
				int newVol = (int)(newPos*100);
				player()->ChangeVolume(newVol, false);
			
			}
			break;
		}

	}
}

void CWndMain::SetLSoftkeyText() {
	int lSoft[4] = {STR_EXIT, STR_MINIMIZE, STR_BLANK, STR_LIBRARY};
	//CreateMenuBar(_str(lSoft[player()->lpConfig->cf.LeftSoftkeyAction]), _str(STR_OPTIONS));

	if (hWndMenuBar) {
		CommandBar_Destroy(hWndMenuBar);
		hWndMenuBar = NULL;
	}

}

void CWndMain::OnMouseUp(int px, int py) {

	switch (ClickMode) {

		case CLK_WHEEL: {

			double cursorRot = atan2(py-getItem(ClickIndex)->IntProp[IWP_YCENTER], px-getItem(ClickIndex)->IntProp[IWP_XCENTER]); // The absolute rotation of the cursor
			deltaRot = cursorRot-oldCursorRot+wheelTurns*2*PI; // deltaRot stores the variation of the rotation value on the wheel

			if (getItem(ClickIndex)->IntProp[wheelClickHalf] == IWA_SEEK && player()->PlayingSongDuration > 0) {
					
				long newPos = (long)(oldPos+deltaRot*25);
				if (newPos > player()->PlayingSongDuration-1) { newPos = player()->PlayingSongDuration-1; }
				if (newPos < 0) { newPos = 0; }

				player()->PlayingSongPos = newPos;

				if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY || MAP_GetStatus(player()->MAP) == MAP_STATUS_PAUSE) {
					MAP_Seek(player()->MAP, newPos*1000);
				}

				UpdatePlayPos();

				if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
					SetTimer(hWnd, TMR_UPDATEPOS, 1000, NULL);
				}

			}
			if (getItem(ClickIndex)->IntProp[wheelClickHalf] == IWA_CHANGEMUSIC && player()->lpConfig->cf.ChangeSongButtons && player()->lpPlaylist->Count > 0) {
				
				if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
					player()->Play();
				} else {
					player()->StopAndClose();
					player()->PlayingIndex = player()->lpPlaylist->Index;
					player()->lpPlaylist->ReadSongDuration();
				}
			}

			while (getItem(ClickIndex)->CustomProp1 > 6.3) { // Constrains the wheel rotation to a smaller value
				getItem(ClickIndex)->CustomProp1 -= 2*PI;
			}
			while (getItem(ClickIndex)->CustomProp1 < 0) { // Constrains the wheel rotation to a smaller value
				getItem(ClickIndex)->CustomProp1 += 2*PI;
			}
			getItem(ClickIndex)->CustomProp = IWM_NORMAL;
			Repaint(PNT_WHEEL, 0);
			break;
		}

		case CLK_WHEELBTN: {

			IWHEELCURPOS iWhlCP = iWheelCurPos(ClickIndex, px, py);
			KillTimer(hWnd, TMR_SECFUNCTION);
			if (iWhlCP == ClickProp) {
				if (iWhlCP == IWC_CBUTTON) {       OnButtonClick(getItem(ClickIndex)->IntProp[IWP_CCOMMAND]); }
				else if (iWhlCP == IWC_LBUTTON) {  OnButtonClick(getItem(ClickIndex)->IntProp[IWP_LCOMMAND]); }
				else if (iWhlCP == IWC_RBUTTON) {  OnButtonClick(getItem(ClickIndex)->IntProp[IWP_RCOMMAND]); }
			}
			getItem(ClickIndex)->CustomProp = IWM_NORMAL;
			Repaint(PNT_WHEEL, 0);
			break;
		}

		case CLK_BUTTON: {

			IBUTTONCURPOS iBtnCP = iButtonCurPos(ClickIndex, px, py);
			KillTimer(hWnd, TMR_SECFUNCTION);
			if (iBtnCP == IBC_INSIDE) {
				OnButtonClick(getItem(ClickIndex)->IntProp[IBP_COMMAND]);
			}
			getItem(ClickIndex)->CustomProp = IBM_NORMAL;
			Repaint(PNT_BUTTON, 0);
			break;
		}

 
		case CLK_TRACK: {
		
			double newPos = iCursorPosToTrackPosition(ClickIndex, px);
			if (getItem(ClickIndex)->IntProp[IRP_TRACKTYPE] == IRT_TIME && player()->PlayingSongDuration > 0) {
			
				int newPlayPos = (int)(newPos*player()->PlayingSongDuration);
				player()->PlayingSongPos = newPlayPos;

				if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY || MAP_GetStatus(player()->MAP) == MAP_STATUS_PAUSE) {
					MAP_Seek(player()->MAP, newPlayPos*1000);
				}

				UpdatePlayPos();
				
				if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
					SetTimer(hWnd, TMR_UPDATEPOS, 1000, NULL);
				}

			} else if (getItem(ClickIndex)->IntProp[IRP_TRACKTYPE] == IRT_VOLUME) {
	
				int newVol = (int)(newPos*100);
				player()->ChangeVolume(newVol, false);
			
			}
			break;
		}
	}

}

void CWndMain::UpdateSysTime() {
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	wchar_t s[6];
	wsprintf(s, L"%02i:%02i", tm.wHour, tm.wMinute);
	iSetText(ITT_SYSTIME, s);
	Repaint(PNT_SYSTIME, 0);
}

void CWndMain::StartSecFncTimer(int command) {
	SecFnc = false;
	SetTimer(hWnd, TMR_SECFUNCTION, 500, NULL);
	if (command != PREV_CMD) {
		SecFncCmd = command;
	}
}

int CWndMain::MenuProc(int uMsg) {
	
	if (uMsg == WBR_MENUSELECTED) {

		if (lpMenu->lpItems == &MenuMain) {
		
			// 0:Media browser
			if (MenuMain.ItemIndex == 0) {
				StopMenu();
				player()->lpBrMedia->Show();
			}

			// 1:Shuffle
			if (MenuMain.ItemIndex == 1) {
				StopMenu();
				player()->lpConfig->cf.Shuffle = !player()->lpConfig->cf.Shuffle;
				iSetCmdToggle(ICMD_SHUFFLE, player()->lpConfig->cf.Shuffle);
				
				player()->lpPlaylist->RebuildShuffle();
			}

			// 2:Repeat
			if (MenuMain.ItemIndex == 2) {
				StopMenu();
				player()->lpConfig->cf.Repeat = !player()->lpConfig->cf.Repeat;
				iSetCmdToggle(ICMD_REPEAT, player()->lpConfig->cf.Repeat);
			}


			// 3:Settings
			if (MenuMain.ItemIndex == 3) {
				MenuSettings.ItemIndex = -1;
				MenuSettings.OffY = 0;
				lpMenu->lpItems = &MenuSettings;
			}

			// 4:About
			if (MenuMain.ItemIndex == 4) {
				StopMenu();
				player()->lpWndAbout->Show();
			}

			// 5:Exit
			if (MenuMain.ItemIndex == 5) {
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}

			// 6:Back to player
			if (MenuMain.ItemIndex == 6) {
				StopMenu();
			}
		}

		if (lpMenu->lpItems == &MenuSettings) {
		
			// 0:General settings
			if (MenuSettings.ItemIndex == 0) {
			
				StopMenu();
				player()->lpDlgSettings->Execute();

			}

			// 1:File associations
			if (MenuSettings.ItemIndex == 1) {
			
				StopMenu();
				player()->lpDlgFileAssoc->Execute();

			}

			// 2:Lists and browsers
			if (MenuSettings.ItemIndex == 2) {
			
				StopMenu();
				player()->lpDlgLists->Execute();

			}

			// 3:Equalizer
			if (MenuSettings.ItemIndex == 3) {
			
				StopMenu();
				player()->lpWndEqualizer->Show();
			}

			// 4:Skin selector
			if (MenuSettings.ItemIndex == 4) {

				StopMenu();
				player()->lpBrSkin->Show();
			
			}

			// 5:Language selector
			if (MenuSettings.ItemIndex == 5) {

				StopMenu();
				player()->lpBrLang->Show();
			
			}

			// 6:Back to previous menu
			if (MenuSettings.ItemIndex == 6) {
				
				MenuMain.ItemIndex = -1;
				MenuMain.OffY = 0;
				lpMenu->lpItems = &MenuMain;

			}

			// 7:Back to player
			if (MenuSettings.ItemIndex == 7) {

				StopMenu();

			}
		}
	}
	return 0;
}

void CWndMain::OnButtonSecFnc(int command) {

	switch (command) {
		
		case ICMD_PREVSONG:
			// Перемотка назад
			if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
				if (MAP_GetCurrent(player()->MAP) > 5000) {
					MAP_Rew(player()->MAP, 4000);
				}
			} else {
				player()->ChangePlaylistIndex(-1, true);
				if (player()->lpConfig->cf.ChangeSongButtons && player()->lpPlaylist->Count > 0) {
					player()->StopAndClose();
					player()->PlayingIndex = player()->lpPlaylist->Index;
					player()->lpPlaylist->ReadSongDuration();
				}
			}
			break;

		case ICMD_NEXTSONG:
			// Перемотка вперед
			if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
				if (MAP_GetCurrent(player()->MAP) < MAP_GetDuration(player()->MAP)-5000) {
					MAP_Ff(player()->MAP, 4000);
				}
			} else {
				player()->ChangePlaylistIndex(+1, true);
				if (player()->lpConfig->cf.ChangeSongButtons && player()->lpPlaylist->Count > 0) {
					player()->StopAndClose();
					player()->PlayingIndex = player()->lpPlaylist->Index;
					player()->lpPlaylist->ReadSongDuration();
				}
			}
			break;

		case ICMD_VOLUMEUP:
			player()->ChangeVolume(+5, true);
			break;

		case ICMD_VOLUMEDOWN:
			player()->ChangeVolume(-5, true);
			break;

		case ICMD_PLAYPAUSE:
			if ((!player()->lpConfig->cf.ChangeSongButtons) && (player()->PlayingIndex >= 0)) {
				player()->ChangePlaylistIndex(player()->PlayingIndex, false);
			}
			break;
	}

}

void CWndMain::OnButtonClick(int command) {

	if (SecFnc) {
		return;
	}
	switch (command) {
		
		case ICMD_PREVSONG:
			if (player()->diskWaitFlag<0) {
				player()->ChangePlaylistIndex(-1, true);
				if (player()->lpConfig->cf.ChangeSongButtons && player()->lpPlaylist->Count > 0) {
					if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
						player()->Play();
					} else {
						player()->StopAndClose();
						player()->PlayingIndex = player()->lpPlaylist->Index;
						player()->lpPlaylist->ReadSongDuration();
					}
				}
			}
			break;

		case ICMD_NEXTSONG:
			if (player()->diskWaitFlag<0) {
				player()->ChangePlaylistIndex(+1, true);
				if (player()->lpConfig->cf.ChangeSongButtons && player()->lpPlaylist->Count > 0) {
					if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
						player()->Play();
					} else {
						player()->StopAndClose();
						player()->PlayingIndex = player()->lpPlaylist->Index;
						player()->lpPlaylist->ReadSongDuration();
					}
				}
			}
			break;

		case ICMD_VOLUMEUP:
			player()->ChangeVolume(+5, true);
			break;

		case ICMD_VOLUMEDOWN:
			player()->ChangeVolume(-5, true);
			break;

		case ICMD_PLAYPAUSE:
			if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY && player()->PlayingIndex == player()->lpPlaylist->Index) {
				player()->Pause();
			} else {
				player()->Play();
			}
			break;

		case ICMD_SETTINGS:
			StartMenu(true);
			break;

		case ICMD_CLOSE:
			player()->PostS100Message(WM_S100_PLATFORM, 2, 0);
			player()->PostS100Message(WM_S100_PLATFORM, 81, 0);
			//SendMessage(hWnd, WM_CLOSE, 0, 0);
			false;

		case ICMD_GOTOCURRENT:
			if ((!player()->lpConfig->cf.ChangeSongButtons) && (player()->PlayingIndex >= 0)) {
				player()->ChangePlaylistIndex(player()->PlayingIndex, false);
			}
			break;

		case ICMD_SLEEPMODE:
			//player()->lpDlgSleep->Execute();
			break;

		case ICMD_EQUALIZER:
#ifdef _PRINT
	CloseHandle(LOG);
	FileID++;
	WCHAR buf[150];
	wsprintf(&buf[0],L"\\Storage Card\\nitrogen_%d.log",FileID);
	while (FileExists(buf)){
		FileID++;
		wsprintf(&buf[0],L"\\Storage Card\\nitrogen_%d.log",FileID);
	}
	LOG = CreateFile(buf, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	MessageBox(hWnd, buf, L"Log", MB_ICONERROR|MB_OK);

#else
			// For S100 standart equalizer uncomment this block and comment next line
			
			player()->PostS100Message(WM_S100_PLATFORM, 2, 0);
			player()->PostS100Message(WM_S100_PLATFORM, 82, 0);
			
			//player()->lpWndEqualizer->Show();
#endif
			
			break;

		case ICMD_MINIMIZE:
			player()->PostS100Message(WM_S100_PLATFORM, 2, 0);
			player()->PostS100Message(WM_S100_PLATFORM, 80, 0);
			//ShowWindow(hWnd, SW_MINIMIZE);
			break;

		case ICMD_BLANKSCREEN:
			//player()->ToggleDisplay(VideoPowerOff);
			break;

		case ICMD_PLAYLIST:
			if (player()->diskWaitFlag>0) {
				KillTimer(hWnd, TMR_DISKWAIT);
				player()->diskWaitFlag=-1;
			}
			player()->lpBrMedia->Show();
			break;

		case ICMD_SHUFFLE:
			player()->lpConfig->cf.Shuffle = !player()->lpConfig->cf.Shuffle;
			iSetCmdToggle(ICMD_SHUFFLE, player()->lpConfig->cf.Shuffle);
			player()->lpPlaylist->RebuildShuffle();
			Repaint(PNT_FORCEFULL, 0);
			break;

		case ICMD_REPEAT:
			player()->lpConfig->cf.Repeat = !player()->lpConfig->cf.Repeat;
			iSetCmdToggle(ICMD_REPEAT, player()->lpConfig->cf.Repeat);
			Repaint(PNT_BTCOMMAND, ICMD_REPEAT);
			break;
	}
}

void CWndMain::StartMenu(bool settings) {

	CreateMenuBar(_str(STR_CANCEL), _str(STR_OK));

	MenuMain.Data[1].Checked = player()->lpConfig->cf.Shuffle;
	MenuMain.Data[2].Checked = player()->lpConfig->cf.Repeat;

	if (!settings) {
		MenuMain.OffY = 0;
		MenuMain.ItemIndex = -1;

		lpMenu->Start(&MenuMain);
	} else {
		MenuSettings.OffY = 0;
		MenuSettings.ItemIndex = -1;

		lpMenu->Start(&MenuSettings);
	}

	ShowingMenu = true;
	InvalidateRect(hWnd, NULL, false);
			
}

void CWndMain::StopMenu() {
			
	SetLSoftkeyText();

	ShowingMenu = false;
	Repaint(PNT_FORCEFULL, 0);
			
}

void CWndMain::OnSoftkeyClick(int softkey) {

	if (softkey == 0) {

		if (!ShowingMenu) {
			switch (player()->lpConfig->cf.LeftSoftkeyAction) {
				
				case SK_EXIT:		
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;

				case SK_BLANK:
					player()->ToggleDisplay(VideoPowerOff);
					break;

				case SK_LIBRARY:
					player()->lpBrMedia->Show();
					break;

				case SK_MINIMIZE:
					ShowWindow(hWnd, SW_MINIMIZE);
					break;

			}
		} else {
			StopMenu();
		}
	}

	if (softkey == 1) {
		
		if (!ShowingMenu) {
			StartMenu(false);
		} else {
			MenuProc(WBR_MENUSELECTED);
		}
	}
}

void CWndMain::OnKeyDown(bool firstPress, int nKey) {

	if ((firstPress) && (nKey < 256)) {
	
		keyPressed[nKey] = true;
		SecFnc = false;
		SetTimer(hWnd, TMR_SECFUNCTION, 500, NULL);

		switch (nKey) {

			case VK_UP:
				SecFncCmd = ICMD_VOLUMEUP;
				break;

			case VK_DOWN:
				SecFncCmd = ICMD_VOLUMEDOWN;
				break;

			case VK_LEFT:
				SecFncCmd = ICMD_PREVSONG;
				break;

			case VK_RIGHT:
				SecFncCmd = ICMD_NEXTSONG;
				break;

			case VK_ACTION:
				SecFncCmd = ICMD_PLAYPAUSE;
				break;

		}
	}
}

void CWndMain::OnKeyUp(int nKey) {

	KillTimer(hWnd, TMR_SECFUNCTION);

	if ((nKey < 256) && (keyPressed[nKey]) && (!SecFnc)) {

		switch (nKey) {

			case VK_ACTION:
				OnButtonClick(ICMD_PLAYPAUSE);
				break;

			case VK_UP:
				OnButtonClick(ICMD_VOLUMEUP);
				break;

			case VK_DOWN:
				OnButtonClick(ICMD_VOLUMEDOWN);
				break;

			case VK_LEFT:
				OnButtonClick(ICMD_PREVSONG);
				if ((!player()->lpConfig->cf.ChangeSongButtons) && (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) && (!player()->IsDisplayOn())) {
					player()->Play();
				}
				break;

			case VK_RIGHT:
				OnButtonClick(ICMD_NEXTSONG);
				if ((!player()->lpConfig->cf.ChangeSongButtons) && (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) && (!player()->IsDisplayOn())) {
					player()->Play();
				}
				break;

		}
	}
	if (nKey < 256) {
		keyPressed[nKey] = false;
	}
}

void CWndMain::UpdateAlbumArt() {

	iDrawAlbumArt(0);
	Repaint(PNT_ALBUMART, 0);

}

void CWndMain::OnTimer(int tmrId) {

	switch (tmrId) {
		case TMR_SECFUNCTION:
			OnButtonSecFnc(SecFncCmd);
			if (!SecFnc) {
				if ((SecFncCmd == ICMD_VOLUMEDOWN) || (SecFncCmd == ICMD_VOLUMEUP)) {
					SetTimer(hWnd, TMR_SECFUNCTION, 200, NULL);
				}
				if ((SecFncCmd == ICMD_PREVSONG) || (SecFncCmd == ICMD_NEXTSONG)) {
					if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
						SetTimer(hWnd, TMR_SECFUNCTION, 1000, NULL);
					} else {
						SetTimer(hWnd, TMR_SECFUNCTION, 200, NULL);
					}
				}
				if (SecFncCmd == ICMD_PLAYPAUSE) {
					KillTimer(hWnd, TMR_SECFUNCTION);
				}
				SecFnc = true;
			}
			break;

		case TMR_UPDATEPOS:
			player()->PlayingSongPos = MAP_GetCurrent(player()->MAP)/1000;
			UpdatePlayPos();
			player()->PostS100Message(WM_S100_AUDIO, 165, player()->PlayingSongPos);
			break;

		case TMR_RESETIDLE:
			SystemIdleTimerReset();
			break;

		case TMR_UPDSYSTIME:
			UpdateSysTime();
			break;

		case TMR_ALBUMART:
			KillTimer(hWnd, TMR_ALBUMART);
			UpdateAlbumArt();
			break;

		case TMR_SLEEP:
			player()->lpDlgSleep->SleepMins--;
			if (player()->lpDlgSleep->SleepMins <= 0) {

				player()->lpDlgSleep->SleepActive = false;	
				KillTimer(hWnd, TMR_SLEEP);
				MenuMain.Data[3].Checked = false;
				iSetCmdToggle(ICMD_SLEEPMODE, false);
				Repaint(PNT_BTCOMMAND, ICMD_SLEEPMODE);

				switch (player()->lpConfig->cf.SleepAction) {
					case STOPMUSIC:
						player()->StopAndClose();
						break;
					case CLOSEAPP:
						SendMessage(hWnd, WM_CLOSE, 0, 0);
						break;
					case STANDBY:
						SetSystemPowerState(NULL, POWER_STATE_SUSPEND, POWER_FORCE);
						break;
				}

			}
			break;

		case TMR_UNLOCKKEYS:
			if (player()->IsDisplayOn()) {
				player()->KeysLocked = false;
				KillTimer(hWnd, TMR_UNLOCKKEYS);
			}
			break;
	}
}

void CWndMain::StartMenuNames() {

	SetWbrItem(&MenuSettings.Data[0], _str(STR_GENERALSETTINGS), ICO_GENERALSET, false, false, false);
	SetWbrItem(&MenuSettings.Data[1], _str(STR_FILEASSOCIATIONS), ICO_FILEASSOC, false, false, false);
	SetWbrItem(&MenuSettings.Data[2], _str(STR_LISTSANDBROWSERS), ICO_LISTS, false, false, false);
	SetWbrItem(&MenuSettings.Data[3], _str(STR_EQUALIZER), ICO_EQUALIZER, false, false, false);
	SetWbrItem(&MenuSettings.Data[4], _str(STR_SKINSELECTOR), ICO_SKINSEL, false, false, false);
	SetWbrItem(&MenuSettings.Data[5], _str(STR_LANGUAGESELECTOR), ICO_LANGSEL, false, true, false);
	SetWbrItem(&MenuSettings.Data[6], _str(STR_BACKTOPREVIOUSME), ICO_BACKTOMENU, false, false, false);
	SetWbrItem(&MenuSettings.Data[7], _str(STR_BACKTOTHEPLAYER), ICO_CLOSEMENU, false, false, false);


	SetWbrItem(&MenuMain.Data[0], _str(STR_MEDIABROWSER), ICO_MEDIABROWSER, false, false, false);
	SetWbrItem(&MenuMain.Data[1], _str(STR_SHUFFLE), ICO_SHUFFLE, false, false, false);
	SetWbrItem(&MenuMain.Data[2], _str(STR_REPEAT), ICO_REPEAT, false, true, false);
	//SetWbrItem(&MenuMain.Data[3], _str(STR_SLEEPMODE), ICO_SLEEPMODE, false, true, false);
	SetWbrItem(&MenuMain.Data[3], _str(STR_SETTINGS), ICO_SETTINGS, false, false, true);
	SetWbrItem(&MenuMain.Data[4], _str(STR_ABOUT), ICO_ABOUT, false, true, false);
	//SetWbrItem(&MenuMain.Data[6], _str(STR_BLANKSCREEN), ICO_BLANKSCREEN, false, false, false);
	SetWbrItem(&MenuMain.Data[5], _str(STR_EXIT), ICO_EXIT, false, false, false);
	SetWbrItem(&MenuMain.Data[6], _str(STR_BACKTOTHEPLAYER), ICO_CLOSEMENU, false, false, false);

}

bool CWndMain::OnCreate() {

#ifdef _PRINT
	WCHAR buf[150];
	wsprintf(&buf[0],L"\\Storage Card\\nitrogen_%d.log",FileID);
	while (FileExists(buf)){
		FileID++;
		wsprintf(&buf[0],L"\\Storage Card\\nitrogen_%d.log",FileID);
	}
	LOG = CreateFile(buf, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
#endif

	MenuSettings.Count = 8;
	MenuSettings.Data = new WBRITEM[8];

	MenuMain.Count = 7;
	MenuMain.Data = new WBRITEM[7];

	StartMenuNames();

	hWndMenuBar = NULL;
	SetLSoftkeyText();
	SetSoftkeyText(hWndMenuBar, 1, _str(STR_OPTIONS));
	MAP_SetMessageWindow(player()->MAP, hWnd);
	MAP_SetEqualizer(player()->MAP, &player()->lpConfig->cf.Equalizer);
	player()->lpConfig->ApplyMAPSettings();
	StartInterface();

#if _WIN32_WCE >= 0x500
	// Starts status handler (for incoming calls)
	RegistryNotifyWindow(SN_PHONEINCOMINGCALL_ROOT, SN_PHONEINCOMINGCALL_PATH, SN_PHONEINCOMINGCALL_VALUE, hWnd, WM_PHONEINCALL, SC_PHONESTATUS, NULL, &hregNotifyPhone);
#endif

	// File association support
	CMDLINE cmd = CMD_NONE;
	wchar_t prCmdLine[MAX_PATH];
	if (wcslen(player()->lpCmdLine) > 0) {
		if (strExtract(prCmdLine, player()->lpCmdLine, 34/*quote*/)) {
			if (CheckFileExt(prCmdLine, L".sk2")) {
				cmd = CMD_SKIN;
			} else {
				cmd = CMD_SONG;
			}
		}
	}

	// Loads playlist
	wchar_t pl[MAX_PATH];
	wsprintf(pl, L"%splaylist.npl", player()->FilesPath);

	if (cmd == CMD_SONG) {
		
		switch (player()->lpConfig->cf.FileAssNotRunning) {

			case MPL_ADDTOPLAYLIST:
				if ((FileExists(pl)) && (!player()->lpPlaylist->LoadFromFile(pl))) {
					MessageBox(hWnd, _str(STR_THEPLAYLISTFILES), _str(STR_ERROR), MB_ICONERROR|MB_OK);
					DeleteFile(pl);
				}
				player()->ChangePlaylistIndex(player()->lpPlaylist->AppendFile(prCmdLine), false);
				break;

			case MPL_SETASPLAYLIST:
				player()->ChangePlaylistIndex(player()->lpPlaylist->AppendFile(prCmdLine), false);
				break;

			case MPL_SETFOLDER: {
				wchar_t lpPath[MAX_PATH];
				ExtractFilePath(lpPath, prCmdLine);
				if (player()->lpConfig->cf.AddSubdirs) {
					player()->lpPlaylist->AppendDirectoryRecursive(lpPath);
				} else {
					player()->lpPlaylist->AppendDirectory(lpPath);
				}
				player()->ChangePlaylistIndex(player()->lpPlaylist->GetFileIndex(prCmdLine), false);
				break;

			}

		}

		if (player()->lpPlaylist->Count > 0) {
			player()->Play();
		}

	} else {
		if ((FileExists(pl)) && (!player()->lpPlaylist->LoadFromFile(pl))) {
			MessageBox(hWnd, _str(STR_THEPLAYLISTFILES), _str(STR_ERROR), MB_ICONERROR|MB_OK);
			DeleteFile(pl);
		}
	}

	iSetCmdToggle(ICMD_SHUFFLE, player()->lpConfig->cf.Shuffle);
	iSetCmdToggle(ICMD_REPEAT, player()->lpConfig->cf.Repeat);
	player()->lpPlaylist->RebuildShuffle();

	// Fixes file association issue (cleans the previous association from 1.0)
	if (player()->lpConfig->Defaults) {
	
		if (player()->lpDlgFileAssoc->IsExtAssigned(L".mp3", L"NitrogenFile")) {
			player()->lpDlgFileAssoc->ExtUnassign(L".mp3");
		}
		if (player()->lpDlgFileAssoc->IsExtAssigned(L".ogg", L"NitrogenFile")) {
			player()->lpDlgFileAssoc->ExtUnassign(L".ogg");
		}
		if (player()->lpDlgFileAssoc->IsExtAssigned(L".sk2", L"NitrogenSkinFile")) {
			player()->lpDlgFileAssoc->ExtUnassign(L".sk2");
		}

		RegDeleteKeyW(HKEY_CLASSES_ROOT, L"NitrogenSkinFile");
		RegDeleteKeyW(HKEY_CLASSES_ROOT, L"NitrogenFile");

	}
	
	// Loads skin
	bool skinLoaded = false;

	if (cmd == CMD_SKIN) {

		wchar_t lpSkinFile[MAX_PATH];
		wchar_t lpSkinPath[MAX_PATH];
		ExtractFileName(lpSkinFile, prCmdLine);
		wsprintf(lpSkinPath, L"%s%s", player()->SkinsPath, lpSkinFile);
		CopyFile(prCmdLine, lpSkinPath, false);
		wcscpy(player()->lpConfig->cf.sSkinFile, lpSkinFile);

	}

	if (player()->lpConfig->Defaults && (cmd != CMD_SKIN)) {

		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		wchar_t DefSkinName[30];
		wsprintf(DefSkinName, L"default-%i-%i.sk2", cx, cy);

		wchar_t DefSkinFilename[MAX_PATH];
		wsprintf(DefSkinFilename, L"%s%s", player()->SkinsPath, DefSkinName);
		wchar_t DefSkinSearch[MAX_PATH];
		wsprintf(DefSkinSearch, L"%sdefault*.sk2", player()->SkinsPath);

		if (iLoadSkin(hWnd, DefSkinFilename, true)) {
			wcscpy(player()->lpConfig->cf.sSkinFile, DefSkinName);
			skinLoaded = true;
			
			WIN32_FIND_DATA fData;
			HANDLE fHandle;
			fHandle = FindFirstFile(DefSkinSearch, &fData);
			if (fHandle != INVALID_HANDLE_VALUE) {
				do {
					if (strCompare(fData.cFileName, DefSkinName) != 0) {
						wchar_t DelSkinFilename[MAX_PATH];
						wsprintf(DelSkinFilename, L"%s%s", player()->SkinsPath, fData.cFileName);
						DeleteFile(DelSkinFilename);
					}
				} while (FindNextFile(fHandle, &fData));
				FindClose(fHandle);

			}
		}
	}

	if (!skinLoaded) {

		wchar_t SkinFilename[MAX_PATH];
		wsprintf(SkinFilename, L"%s%s", player()->SkinsPath, player()->lpConfig->cf.sSkinFile);
		if (!iLoadSkin(hWnd, SkinFilename, true)) {
			
			MessageBox(hWnd, _str(STR_COULDNOTLOADTHES), _str(STR_MESSAGE), MB_ICONINFORMATION|MB_OK);
			
			WIN32_FIND_DATA fData;
			HANDLE fHandle;

			wchar_t SkinSearch[MAX_PATH];
			wsprintf(SkinSearch, L"%s*.sk2", player()->SkinsPath);
			fHandle = FindFirstFile(SkinSearch, &fData);
			
			if (fHandle == INVALID_HANDLE_VALUE) {
			
				MessageBox(hWnd, _str(STR_THEREARENOSKINFI), _str(STR_ERROR), MB_ICONERROR|MB_OK);
				return false;

			} else {
				wchar_t sName[100];
				bool suc = false;			
				do {
					wchar_t AltSkinFilename[MAX_PATH];
					wsprintf(AltSkinFilename, L"%s%s", player()->SkinsPath, fData.cFileName);
					suc = iLoadSkin(hWnd, AltSkinFilename, true);
					if (suc) {
						wcscpy(sName, fData.cFileName);
						break;
					}
				} while (FindNextFile(fHandle, &fData));
				FindClose(fHandle);
				
				if (suc) {
					wcscpy(player()->lpConfig->cf.sSkinFile, sName);
				} else {
					MessageBox(hWnd, _str(STR_NONEOFTHEFILESIN), _str(STR_ERROR), MB_ICONERROR|MB_OK);
					return false;
				}
			}
		}
	}

	lpMenu = new CMenu(hWnd, (MENUPROC)CWndMainMenuProc);

	player()->ChangeVolume(player()->lpConfig->cf.Volume, false);

	UpdateSysTime();
	SetTimer(hWnd, TMR_UPDSYSTIME, 30000, NULL);

	UpdatePlayPos();

	memset(&keyPressed, 0, sizeof(keyPressed));

	HKEY hKey;
	if (player()->lpConfig->cf.WriteSongRegistry && RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"System\\State\\Nitrogen", 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, NULL) == ERROR_SUCCESS) {
	
		DWORD buf = 1;
		RegSetValueExW(hKey, L"Running", 0, REG_DWORD, (BYTE*)&buf, 4);
		RegCloseKey(hKey);

	}
	PostMessage(hWnd,WM_COMMAND,ECMD_PLAY,0);
	return true;
}

void CWndMain::OnCopyData(LPVOID lpData) {

	LPWSTR fName = (LPWSTR)lpData;

	if (CheckFileExt(fName, L".sk2")) {
		
		wchar_t lpSkinFile[MAX_PATH];
		wchar_t lpSkinPath[MAX_PATH];
		ExtractFileName(lpSkinFile, fName);
		wsprintf(lpSkinPath, L"%s%s", player()->SkinsPath, lpSkinFile);
		CopyFile(fName, lpSkinPath, false);

		if (iLoadSkin(hWnd, lpSkinPath, false)) {

			wcscpy(player()->lpConfig->cf.sSkinFile, lpSkinFile);
			for (int i = 0; i < getItemsCount(); i++) {
				if (getItem(i)->Kind == IIK_TRACKBAR) {
					iDrawTrackbar(NULL, i, true);
				}
			}
			lpMenu->UpdateMenuSize();
			UpdateAlbumArt();
		}

		Repaint(PNT_FORCEFULL, 0);
	
	} else {

		switch (player()->lpConfig->cf.FileAssRunning) {

			case MPL_ADDTOPLAYLIST:
				player()->ChangePlaylistIndex(player()->lpPlaylist->AppendFile(fName), false);
				break;

			case MPL_SETASPLAYLIST:
				player()->lpPlaylist->Clear();
				player()->ChangePlaylistIndex(player()->lpPlaylist->AppendFile(fName), false);
				break;

			case MPL_SETFOLDER: {
				player()->lpPlaylist->Clear();
				wchar_t lpPath[MAX_PATH];
				ExtractFilePath(lpPath, fName);
				if (player()->lpConfig->cf.AddSubdirs) {
					player()->lpPlaylist->AppendDirectoryRecursive(lpPath);
				} else {
					player()->lpPlaylist->AppendDirectory(lpPath);
				}
				player()->ChangePlaylistIndex(player()->lpPlaylist->GetFileIndex(fName), false);
				break;

			}

		}

		player()->lpPlaylist->RebuildShuffle();

		if (player()->lpPlaylist->Count > 0) {
			player()->Play();
		}
	
	} 
}

void CWndMain::OnPaint(HDC hDC) {
	
	if (paintMode == PNT_FORCEFULL) {
		paintMode = PNT_FULL;
	}

	if (paintMode == PNT_FULL) {
		iDrawFull(hDC);
	}
	
	if (paintMode & PNT_WHEEL) {
		if (((paintMode|PNT_BTCOMMAND) != paintMode) || (((paintMode|PNT_BTCOMMAND) == paintMode) && (paintCommand[getItem(ClickIndex)->IntProp[IWP_CCOMMAND]] == false) && (getItem(ClickIndex)->IntProp[IWP_LCOMMAND] == false) && (getItem(ClickIndex)->IntProp[IWP_RCOMMAND] == false))) {
			iDrawWheel(hDC, ClickIndex, 0, 0);
		}
	}

	if (paintMode & PNT_PLAYBACK) {
		for (int i = 0; i < getItemsCount(); i++) {
			if ((getItem(i)->Kind == IIK_BUTTON) && ((getItem(i)->IntProp[IBP_COMMAND] == ICMD_NEXTSONG) || (getItem(i)->IntProp[IBP_COMMAND] == ICMD_PREVSONG) || (getItem(i)->IntProp[IBP_COMMAND] == ICMD_PLAYPAUSE))) {
				iDrawButton(hDC, i, 0, 0);
			} else if ((getItem(i)->Kind == IIK_WHEEL) && ((getItem(i)->IntProp[IWP_LCOMMAND] == ICMD_NEXTSONG) || (getItem(i)->IntProp[IWP_LCOMMAND] == ICMD_PREVSONG) || (getItem(i)->IntProp[IWP_LCOMMAND] == ICMD_PLAYPAUSE) || (getItem(i)->IntProp[IWP_CCOMMAND] == ICMD_NEXTSONG) || (getItem(i)->IntProp[IWP_CCOMMAND] == ICMD_PREVSONG) || (getItem(i)->IntProp[IWP_CCOMMAND] == ICMD_PLAYPAUSE) || (getItem(i)->IntProp[IWP_RCOMMAND] == ICMD_NEXTSONG) || (getItem(i)->IntProp[IWP_RCOMMAND] == ICMD_PREVSONG) || (getItem(i)->IntProp[IWP_RCOMMAND] == ICMD_PLAYPAUSE))) {
				iDrawWheel(hDC, i, 0, 0);
			}
		}
	}

	if (paintMode & PNT_SONGINFO) {
		for (int i = 0; i < getItemsCount(); i++) {
			if ((getItem(i)->Kind == IIK_TEXT) && ((getItem(i)->IntProp[ITP_TEXTTYPE] == ITT_SONGNAME) || (getItem(i)->IntProp[ITP_TEXTTYPE] == ITT_SONGARTIST) || (getItem(i)->IntProp[ITP_TEXTTYPE] == ITT_TRACKNUMBER) || (getItem(i)->IntProp[ITP_TEXTTYPE] == ITT_TRACKCOUNT))) {
				iDrawTextBg(hDC, i, 0, 0);
			}
		}
	}

	if (paintMode & PNT_ALBUMART) {
		iDrawAlbumArt(hDC);
	}

	if (paintMode & PNT_VOLUME) {
		HDC hBufDC = CreateCompatibleDC(NULL);
		HBITMAP hBufBmp = CreateBitmap(get_devWid(), get_devHei(), 1, 16, NULL);
		HBITMAP hBufBmpOld = (HBITMAP)SelectObject(hBufDC, hBufBmp);
		BitBlt(hBufDC, 0, 0, get_devWid(), get_devHei(), hDC, 0, 0, SRCCOPY);
		for (int i = 0; i < getItemsCount(); i++) {
			if ((getItem(i)->Kind == IIK_TRACKBAR) && (getItem(i)->IntProp[IRP_TRACKTYPE] == IRT_VOLUME)) {
				iDrawTrackbar(hBufDC, i, false);
				LPRECT prect = getRect(getItem(i)->IntProp[IRP_XPOS], getItem(i)->IntProp[IRP_YPOS], getItem(i)->IntProp[IRP_XPOS]+getItem(i)->IntProp[IRP_WIDTH], getItem(i)->IntProp[IRP_YPOS]+getItem(i)->IntProp[IRP_HEIGHT]);
				iDrawTexts(hBufDC, prect, -1);
				delete prect;
			}
		}
		iDrawTexts(hBufDC, NULL, ITT_VOLUME);
		BitBlt(hDC, 0, 0, get_devWid(), get_devHei(), hBufDC, 0, 0, SRCCOPY);
		SelectObject(hBufDC, hBufBmpOld);
		DeleteObject(hBufBmp);
		DeleteDC(hBufDC);
	}

	if (paintMode & PNT_TIME) {
		HDC hBufDC = CreateCompatibleDC(NULL);
		HBITMAP hBufBmp = CreateBitmap(get_devWid(), get_devHei(), 1, 16, NULL);
		HBITMAP hBufBmpOld = (HBITMAP)SelectObject(hBufDC, hBufBmp);
		BitBlt(hBufDC, 0, 0, get_devWid(), get_devHei(), hDC, 0, 0, SRCCOPY);
		for (int i = 0; i < getItemsCount(); i++) {
			if ((getItem(i)->Kind == IIK_TRACKBAR) && (getItem(i)->IntProp[IRP_TRACKTYPE] == IRT_TIME)) {
				iDrawTrackbar(hBufDC, i, false);
				LPRECT prect = getRect(getItem(i)->IntProp[IRP_XPOS], getItem(i)->IntProp[IRP_YPOS], getItem(i)->IntProp[IRP_XPOS]+getItem(i)->IntProp[IRP_WIDTH], getItem(i)->IntProp[IRP_YPOS]+getItem(i)->IntProp[IRP_HEIGHT]);
				iDrawTexts(hBufDC, prect, -1);
				delete prect;
			}
		}
		iDrawTexts(hBufDC, NULL, ITT_ELAPSED);
		iDrawTexts(hBufDC, NULL, ITT_REMAINING);
		BitBlt(hDC, 0, 0, get_devWid(), get_devHei(), hBufDC, 0, 0, SRCCOPY);
		SelectObject(hBufDC, hBufBmpOld);
		DeleteObject(hBufBmp);
		DeleteDC(hBufDC);
	}

	if (paintMode & PNT_SYSTIME) {
		for (int i = 0; i < getItemsCount(); i++) {
			if ((getItem(i)->Kind == IIK_TEXT) && (getItem(i)->IntProp[ITP_TEXTTYPE] == ITT_SYSTIME)) {
				iDrawTextBg(hDC, i, 0, 0);
			}
		}
	}

	if (paintMode & PNT_BUTTON) {
		if (((paintMode|PNT_BTCOMMAND) != paintMode) || (((paintMode|PNT_BTCOMMAND) == paintMode) && (paintCommand[getItem(ClickIndex)->IntProp[IBP_COMMAND]] == false))) {
			iDrawButton(hDC, ClickIndex, 0, 0);
		}
	}

	if (paintMode & PNT_BTCOMMAND) {
		for (int i = 0; i < getItemsCount(); i++) {
			if ((getItem(i)->Kind == IIK_BUTTON) && (paintCommand[getItem(i)->IntProp[IBP_COMMAND]] == true)) {
				iDrawButton(hDC, i, 0, 0);
			} else if ((getItem(i)->Kind == IIK_WHEEL) && ((paintCommand[getItem(i)->IntProp[IWP_CCOMMAND]] == true) || (paintCommand[getItem(i)->IntProp[IWP_RCOMMAND]] == true) || (paintCommand[getItem(i)->IntProp[IWP_LCOMMAND]] == true))) {
				iDrawWheel(hDC, i, 0, 0);
			}
		}
	}

	for (int i = 0; i < 40; i++) {
		paintCommand[i] = false;
	}

	paintMode = PNT_FULL;
}

void CWndMain::UpdatePlayPos(bool UpdateReg) {

	if (player()->PlayingSongDuration > 0) {
		iSetTrackPosition(IRT_TIME, (double)player()->PlayingSongPos/player()->PlayingSongDuration);
	} else {
		iSetTrackPosition(IRT_TIME, 0);
	}
	int rmSec = player()->PlayingSongDuration-player()->PlayingSongPos;
	if (rmSec < 0) {
		rmSec = 0;
	}
	wchar_t sElapsed[6];
	SecsToTime(sElapsed, player()->PlayingSongPos);
	wchar_t sRemaining[6];
	SecsToTime(sRemaining, rmSec);
	iSetText(ITT_ELAPSED, sElapsed);
	iSetText(ITT_REMAINING, sRemaining);

	HKEY hKey;
	if (UpdateReg && player()->lpConfig->cf.WriteSongRegistry && RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"System\\State\\Nitrogen", 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, NULL) == ERROR_SUCCESS) {
	
		RegSetValueExW(hKey, L"TimeElapsed", 0, REG_DWORD, (BYTE*)&player()->PlayingSongPos, 4);
		RegSetValueExW(hKey, L"TimeTotal", 0, REG_DWORD, (BYTE*)&player()->PlayingSongDuration, 4);
		RegCloseKey(hKey);

	}

	Repaint(PNT_TIME, 0);

}

void CWndMain::Repaint(int mode, int cmd) {
	
	paintMode = paintMode | mode;
	if (mode & PNT_BTCOMMAND) {
		paintCommand[cmd] = true;
	}
	if (Showing) {
		InvalidateRect(hWnd, NULL, false);
	}

}

void CWndMain::OnExternalCmd(DWORD uCmd) {

	switch (uCmd) {

		case ECMD_PLAYPAUSE:	
			if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY && player()->PlayingIndex == player()->lpPlaylist->Index) {
				player()->Pause();
			} else {
				player()->Play();
			}
			break;

		case ECMD_PAUSE:	
			if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY && player()->PlayingIndex == player()->lpPlaylist->Index) {
				player()->Pause();
			}
			break;

		case ECMD_PLAY:	
			player()->Play();
			break;

		case ECMD_PREVSONG:
			player()->ChangePlaylistIndex(-1, true);
			if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
				player()->Play();
			} else {
				player()->StopAndClose();
				player()->PlayingIndex = player()->lpPlaylist->Index;
				player()->lpPlaylist->ReadSongDuration();
			}
			break;

		case ECMD_NEXTSONG:
			player()->ChangePlaylistIndex(+1, true);
			if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
				player()->Play();
			} else {
				player()->StopAndClose();
				player()->PlayingIndex = player()->lpPlaylist->Index;
				player()->lpPlaylist->ReadSongDuration();
			}
			break;

		case ECMD_VOLUMEUP:
			player()->ChangeVolume(+5, true);
			break;

		case ECMD_VOLUMEDOWN:
			player()->ChangeVolume(-5, true);
			break;

	}

}

LRESULT CWndMain::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
#ifdef _PRINT
	DWORD n;
	WCHAR buf[150];
	wsprintf(&buf[0],L"WM_MESSAAGE: %d wParam = %d lParam=%d\n", uMsg, wParam, lParam);
	WriteFile(LOG, &buf[0], wcslen(&buf[0])*sizeof(WCHAR), &n, false);
#endif

	switch (uMsg) {
		case WM_LBUTTONDOWN:
#ifdef _PRINT
	wsprintf(&buf[0],L"WM_LBUTTONDOWN: wParam = %d lParam=%d\n", wParam, lParam);
	WriteFile(LOG, &buf[0], wcslen(&buf[0])*sizeof(WCHAR), &n, false);
#endif
			if (wParam>0) {
				if (ShowingMenu) {	lpMenu->OnMouseDown(LOWORD(lParam), HIWORD(lParam));	}
				else {	OnMouseDown(LOWORD(lParam), HIWORD(lParam));	}
			}
			break;

		case WM_MOUSEMOVE:
#ifdef _PRINT
	wsprintf(&buf[0],L"WM_MOUSEMOVE: wParam = %d lParam=%d\n", wParam, lParam);
	WriteFile(LOG, &buf[0], wcslen(&buf[0])*sizeof(WCHAR), &n, false);
#endif
			if (wParam>0) {
				if (ShowingMenu) {	lpMenu->OnMouseMove(LOWORD(lParam), HIWORD(lParam));	}
				else {				OnMouseMove(LOWORD(lParam), HIWORD(lParam));			}
			}
			break;

		case WM_LBUTTONUP:
#ifdef _PRINT
	wsprintf(&buf[0],L"WM_LBUTTONUP: wParam = %d lParam=%d\n", wParam, lParam);
	WriteFile(LOG, &buf[0], wcslen(&buf[0])*sizeof(WCHAR), &n, false);
#endif
			if (ShowingMenu) {	lpMenu->OnMouseUp(LOWORD(lParam), HIWORD(lParam));		}
			else {	OnMouseUp(LOWORD(lParam), HIWORD(lParam));	}
			break;

		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			
			if (wmId == IDM_LSOFT) {		 OnSoftkeyClick(0);		}
			else if (wmId == IDM_RSOFT) {	 OnSoftkeyClick(1);		}

			if ((wParam >= 40001) && (wParam < 40010)) {
				OnExternalCmd(wParam);
			}
			break;
		}

		case WM_S100_AUDIO: { // S100 команды взаимодействия с процессом Platform.exe

#ifdef _PRINT
	wsprintf(&buf[0],L"WM_S100: wParam = %d lParam=%d\n",wParam,lParam);
	WriteFile(LOG, &buf[0], wcslen(&buf[0])*sizeof(WCHAR), &n, false);
#endif
			switch (wParam) {
				case 4: {// Play_Pause
					OnExternalCmd(ECMD_PLAYPAUSE);				
					break;
				}
				case 5: {// Prev
					OnExternalCmd(ECMD_PREVSONG);				
					break;
				}
				case 7: {// Next
					OnExternalCmd(ECMD_NEXTSONG);
					break;
				}
				case 8: {// Скорее всего надо завершить процесс
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				}

				case 166: {// lParam=33 Возникает при прерывании потока аудио
					//MessageBox(hWnd, L"Message 166", _str(STR_MESSAGE), MB_ICONINFORMATION|MB_OK|MB_SETFOREGROUND);
					break;
				}
				case 175: {// Запрос текущего состояния плеера
					player()->PostS100StatusMessage();
					break;
				}
				default: {
					break;
				}
			}
			break;
		}

		case WM_KEYDOWN: {
#ifdef _PRINT
	wsprintf(&buf[0],L"WM_KEYDOWN: wParam = %d lParam=%d\n", wParam, lParam);
	WriteFile(LOG, &buf[0], wcslen(&buf[0])*sizeof(WCHAR), &n, false);
#endif
			bool firstPress = ((lParam & 0x40000000) == 0);
			if (ShowingMenu) {
				lpMenu->OnKeyDown(firstPress, wParam);
			} else {
				OnKeyDown(firstPress, wParam);
			}
			break;
		}

		case WM_KEYUP:
#ifdef _PRINT
	wsprintf(&buf[0],L"WM_KEYUP: wParam = %d lParam=%d\n", wParam, lParam);
	WriteFile(LOG, &buf[0], wcslen(&buf[0])*sizeof(WCHAR), &n, false);
#endif
			if (ShowingMenu) {
				lpMenu->OnKeyUp(wParam);
			} else {
				OnKeyUp(wParam);
			}
			break;

		case WM_TIMER:
			OnTimer(wParam);
			break;

		case MAP_MSG_STATUS:
			player()->OnChangePlayerStatus((MAP_STATUS)wParam);
			break;

		case MAP_MSG_EOF:
			player()->OnSongEOF();
			break;

		case WM_CREATE:
			if (!OnCreate()) {
				return -1;
			}
			break;

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

		case WM_DESTROY: {
#ifdef _PRINT
CloseHandle(LOG);
#endif

#if _WIN32_WCE >= 0x500
			RegistryCloseNotification(hregNotifyPhone);
#endif
			bool Playing = (player()->PlayingIndex >= 0) && (player()->PlayingSongDuration > 0) && (player()->PlayingSongPos > 0);
			int PlayingIndex = 0;
			DWORD CurrentPos = 0;

			if (Playing) {
				PlayingIndex = player()->PlayingIndex;
				CurrentPos = player()->PlayingSongPos;
			}
			player()->StopAndClose();

			HKEY hKey;
			if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"System\\State\\Nitrogen", 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, NULL) == ERROR_SUCCESS) {
	
				DWORD buf = 0;
				RegSetValueExW(hKey, L"Running", 0, REG_DWORD, (BYTE*)&buf, 4);
				RegCloseKey(hKey);

			}
		
			wchar_t pl[MAX_PATH];
			wsprintf(pl, L"%splaylist.npl", player()->FilesPath);
			player()->lpPlaylist->SaveToFile(pl, Playing, PlayingIndex, CurrentPos);

			wchar_t cf[MAX_PATH];
			wsprintf(cf, L"%sconfig.dat", player()->FilesPath);
			MAP_GetEqualizer(player()->MAP, &player()->lpConfig->cf.Equalizer);
			player()->lpConfig->SaveToFile(cf);

			MAP_Uninitialize(player()->MAP);
			CommandBar_Destroy(hWndMenuBar);
			PostQuitMessage(0);
			break;
		}

		case WM_ERASEBKGND:
			Repaint(PNT_FORCEFULL, 0);
			break;

		case WM_ACTIVATE:
			if (wParam != WA_INACTIVE) {
				Repaint(PNT_FORCEFULL, 0);
				Showing = true;
			} else {
				Showing = false;
			}
			break;

		case WM_COPYDATA: {
			COPYDATASTRUCT * cds = (COPYDATASTRUCT*)lParam;
			if (cds->dwData == 3452) {
				OnCopyData(cds->lpData);
			}
			return true;
		}
/*
		case WM_NTG_ISPLAYING:
			if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
				return 1;
			}
			return 0;

		case WM_NTG_GETCURRENT:
			return MAKELONG(player()->PlayingSongPos, player()->PlayingSongDuration);

		case WM_NTG_SETCURRENT: {
			if (player()->PlayingIndex >= 0 && player()->PlayingSongDuration > 0) {
				player()->PlayingSongPos = wParam;
				if (player()->PlayingSongPos >= player()->PlayingSongDuration) { player()->PlayingSongPos = player()->PlayingSongDuration-1; }
				if (player()->PlayingSongPos < 0) { player()->PlayingSongPos = 0; }
				if (MAP_GetStatus(player()->MAP) == MAP_STATUS_PAUSE || MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY) {
					MAP_Seek(player()->MAP, wParam*1000);
				}
				UpdatePlayPos();
				return 1;
			}
			return 0;
		}

		case WM_NTG_GETVOLUME:
			return player()->lpConfig->cf.Volume;

		case WM_NTG_SETVOLUME:
			player()->ChangeVolume((int)wParam, (lParam == 1));
			return 0;

		case WM_NTG_GETMUSICCODE:
			return player()->musicCode;
*/
#if _WIN32_WCE >= 0x500
		case WM_PHONEINCALL:
			// In-call handling

			if (lParam == SC_PHONESTATUS) {

				if (wParam & SN_PHONEINCOMINGCALL_BITMASK) {
					inCallPlaying = (MAP_GetStatus(player()->MAP) == MAP_STATUS_PLAY);
					player()->Pause();
				} else {
					if (inCallPlaying) {
						player()->Play();
					}
				}

			}
			break;
#endif


		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK CWndMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CREATE) {
		player()->lpWndMain->hWnd = hWnd;
	}
	return player()->lpWndMain->WndProc(uMsg, wParam, lParam);
}

int CWndMainMenuProc(int uMsg) {
	return player()->lpWndMain->MenuProc(uMsg);
}

CWndMain::~CWndMain() {

	delete lpMenu;

}
