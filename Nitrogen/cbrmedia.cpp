#include "stdafx.h"
#include "main.h"

void CBrMedia::StartMenuNames() {

	SetWbrItem(&MenuMediaTap.Data[0], _str(STR_APPENDITEMANDPLA), ICO_ADDSONG, false, false, false);
	SetWbrItem(&MenuMediaTap.Data[1], _str(STR_ENQUEUEITEM), ICO_ADDASNEXT, false, true, false);
	SetWbrItem(&MenuMediaTap.Data[2], _str(STR_PLAYCURRENTFOLDE), ICO_SETFOLDER, false, false, false);
	SetWbrItem(&MenuMediaTap.Data[3], _str(STR_APPENDCURRENTFOL), ICO_ADDFOLDER, false, true, false);
	SetWbrItem(&MenuMediaTap.Data[4], _str(STR_RECURSIVESEARCH), ICO_RECURSIVE, false, false, false);
	SetWbrItem(&MenuMediaTap.Data[5], _str(STR_CANCEL), ICO_CLOSEMENU, false, false, false);

	SetWbrItem(&MenuMedia.Data[0], _str(STR_PLAYCURRENTFOLDE), ICO_SETFOLDER, false, false, false);
	SetWbrItem(&MenuMedia.Data[1], _str(STR_APPENDCURRENTFOL), ICO_ADDFOLDER, false, false, false);
	SetWbrItem(&MenuMedia.Data[2], _str(STR_PLAYSELECTION), ICO_SETSELECTION, false, false, false);
	SetWbrItem(&MenuMedia.Data[3], _str(STR_APPENDSELECTION), ICO_ADDSELECTION, false, true, false);
	SetWbrItem(&MenuMedia.Data[4], _str(STR_RECURSIVESEARCH), ICO_RECURSIVE, false, true, false);
	SetWbrItem(&MenuMedia.Data[5], _str(STR_SELECTALL), ICO_SELECTALL, false, false, false);
	SetWbrItem(&MenuMedia.Data[6], _str(STR_SELECTNONE), ICO_SELECTNONE, false, false, false);
	SetWbrItem(&MenuMedia.Data[7], _str(STR_INVERTSELECTION), ICO_SELECTINV, false, true, false);
	SetWbrItem(&MenuMedia.Data[8], _str(STR_BACKTOTHEPLAYER), ICO_CLOSEMENU, false, false, false);

	SetWbrItem(&MenuPlaylist.Data[0], _str(STR_DELETESELECTED), ICO_DELCHECKED, false, false, false);
	SetWbrItem(&MenuPlaylist.Data[1], _str(STR_CLEARPLAYLIST), ICO_CLEARPL, false, true, false);
	SetWbrItem(&MenuPlaylist.Data[2], _str(STR_GOTOPLAYINGSONG), ICO_GOTOCURRENT, false, true, false);
	SetWbrItem(&MenuPlaylist.Data[3], _str(STR_SELECTALL), ICO_SELECTALL, false, false, false);
	SetWbrItem(&MenuPlaylist.Data[4], _str(STR_SELECTNONE), ICO_SELECTNONE, false, false, false);
	SetWbrItem(&MenuPlaylist.Data[5], _str(STR_INVERTSELECTION), ICO_SELECTINV, false, true, false);
	SetWbrItem(&MenuPlaylist.Data[6], _str(STR_BACKTOTHEPLAYER), ICO_CLOSEMENU, false, false, false);

	SetWbrItem(&MenuPlaylistTap.Data[0], _str(STR_PLAYSELECTION), ICO_PLAYNOW, false, false, false);
	SetWbrItem(&MenuPlaylistTap.Data[1], _str(STR_SETASNEXT), ICO_ADDASNEXT, false, true, false);
	SetWbrItem(&MenuPlaylistTap.Data[2], _str(STR_DELETE), ICO_DELETEITEM, false, true, false);
	SetWbrItem(&MenuPlaylistTap.Data[3], _str(STR_MOVEUP), ICO_MOVEUP, false, false, false);
	SetWbrItem(&MenuPlaylistTap.Data[4], _str(STR_MOVEDOWN), ICO_MOVEDOWN, false, true, false);
	SetWbrItem(&MenuPlaylistTap.Data[5], _str(STR_MOVETO), ICO_MOVETO, false, true, false);
	SetWbrItem(&MenuPlaylistTap.Data[6], _str(STR_GOTOFOLDER), ICO_GOTOFOLDER, false, false, false);
	SetWbrItem(&MenuPlaylistTap.Data[7], _str(STR_CANCEL), ICO_CLOSEMENU, false, false, false);

	SetWbrItem(&MenuMoveToTap.Data[0], _str(STR_BEFORETHISITEM), ICO_BEFORETHIS, false, false, false);
	SetWbrItem(&MenuMoveToTap.Data[1], _str(STR_AFTERTHISITEM), ICO_AFTERTHIS, false, true, false);
	SetWbrItem(&MenuMoveToTap.Data[2], _str(STR_SWAP), ICO_SWAP, false, true, false);
	SetWbrItem(&MenuMoveToTap.Data[3], _str(STR_CANCELMOVE), ICO_CANCEL, false, false, false);

	SetWbrItem(&MenuMoveTo.Data[0], _str(STR_CANCELMOVE), ICO_CANCEL, false, false, false);

	wcscpy(ListPlaylist.Title, _str(STR_PLAYLIST));
}

CBrMedia::CBrMedia() {
	
	Showing = false;

	MenuMediaTap.Count = 6;
	MenuMediaTap.Data = new WBRITEM[6];

	MenuMedia.Count = 9;
	MenuMedia.Data = new WBRITEM[9];

	MenuPlaylist.Count = 7;
	MenuPlaylist.Data = new WBRITEM[7];

	MenuPlaylistTap.Count = 8;
	MenuPlaylistTap.Data = new WBRITEM[8];

	MenuMoveToTap.Count = 4;
	MenuMoveToTap.Data = new WBRITEM[4];

	MenuMoveTo.Count = 1;
	MenuMoveTo.Data = new WBRITEM[1];

	StartMenuNames();

	ListMedia.CheckEnabled = true;
	ListMedia.OffY = 0;
	ListMedia.ItemIndex = -1;
	ListMedia.Data = NULL;
	ListMedia.TitleIcon = ICO_DIRUP;

	ListPlaylist.CheckEnabled = true;
	ListPlaylist.OffY = 0;
	ListPlaylist.ItemIndex = -1;
	ListPlaylist.Data = NULL;
	ListPlaylist.TitleIcon = -1;

	Mode = CBR_MEDIA;

}

void GetUpDir(LPWSTR lpOut, LPWSTR lpPath) {
	if (wcslen(lpPath) == 1) {
		wcscpy(lpOut, lpPath);
	} else {
		int lastPos = -1;
		for (int i = 0; i < wcslen(lpPath)-1; i++) {
			if (lpPath[i] == 92) {
				lastPos = i;
			}
		}
		if (lastPos == -1) {
			wcscpy(lpOut, lpPath);
		} else {
			lpOut[lastPos+1] = 0;
			for (int i = 0; i <= lastPos; i++) {
				lpOut[i] = lpPath[i];
			}
		}
	}
}


void CBrMedia::ListPlaylistItems() {
	ClearWbrList(&ListPlaylist);
	ListPlaylist.Count = player()->lpPlaylist->Count;
	ListPlaylist.Data = new WBRITEM[ListPlaylist.Count];
	for (int i = 0; i < player()->lpPlaylist->Count; i++) {
		ListPlaylist.Data[i].Arrow = false;
		ListPlaylist.Data[i].Checked = false;
		if (player()->PlayingIndex == i) {
			ListPlaylist.Data[i].IconIndex = ICO_PLITEMPLAY;
		} else if (player()->lpPlaylist->Index == i) {
			ListPlaylist.Data[i].IconIndex = ICO_PLITEMNEXT;
		} else {
			ListPlaylist.Data[i].IconIndex = ICO_PLITEM;
		}
		ListPlaylist.Data[i].Separator = false;
		
		ExtractFileName(ListPlaylist.Data[i].Text, player()->lpPlaylist->Data[i].FileName);
		ListPlaylist.Data[i].TextLength = wcslen(ListPlaylist.Data[i].Text);
	}
	ListPlaylist.ItemIndex = player()->lpPlaylist->Index;
}

void CBrMedia::ListDirFiles(LPWSTR path) {
	WIN32_FIND_DATA findData;
	HANDLE fileHandle;

	wchar_t dirSearch[MAX_PATH];
	wsprintf(dirSearch, L"%s*", path);

	// Searches for directory count
	fileHandle = FindFirstFile(dirSearch, &findData);
	int cntDir = 0;
	if (fileHandle != INVALID_HANDLE_VALUE) {
		do {
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				cntDir++;
			}
		} while (FindNextFile(fileHandle, &findData));
		FindClose(fileHandle);
	}
	
	// Searches for media count
	int cntMedia = 0;
	for (int i = 0; i < player()->ExtCount; i++) {
		wchar_t search[MAX_PATH];
		wsprintf(search, L"%s*.%s", path, player()->ExtNames[i]);
		cntMedia += GetFileCount(search);
	}

	ClearWbrList(&ListMedia);
	wcscpy(ListMedia.Title, path);
	ListMedia.ItemIndex = -1;

	ListMedia.Count = cntDir+cntMedia;
	
	if (ListMedia.Count > 0) {
		ListMedia.Data = new WBRITEM[ListMedia.Count];
	} else {
		ListMedia.Data = NULL;
	}
	DirCount = cntDir;

	FindClose(fileHandle);

	// Copies directories to array
	if (cntDir > 0) {
		fileHandle = FindFirstFile(dirSearch, &findData);
		int i = 0;
		do {
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				SetWbrItem(&ListMedia.Data[i], findData.cFileName, ICO_DIRECTORY, false, false, true);
				i++;
			}
		} while (FindNextFile(fileHandle, &findData));
		FindClose(fileHandle);
	}

	// Copies files to array
	if (cntMedia > 0) {
	
		int ind = DirCount;

		for (int i = 0; i < player()->ExtCount; i++) {
			wchar_t search[MAX_PATH];
			wsprintf(search, L"%s*.%s", path, player()->ExtNames[i]);

			fileHandle = FindFirstFile(search, &findData);
			if (fileHandle != INVALID_HANDLE_VALUE) {
	
				do {
					SetWbrItem(&ListMedia.Data[ind], findData.cFileName, ICO_SSONGFILE, false, false, false);
					ind++;
				} while (FindNextFile(fileHandle, &findData));
				FindClose(fileHandle);

			}
		}

	}

	for (int i = 0; i < ListMedia.Count; i++) {
		for (int x = i+1; x < ListMedia.Count; x++) {
			if ((ListMedia.Data[i].IconIndex == ListMedia.Data[x].IconIndex) && (strCompare(ListMedia.Data[i].Text, ListMedia.Data[x].Text) > 0)) {
				WBRITEM wi = ListMedia.Data[x];
				ListMedia.Data[x] = ListMedia.Data[i];
				ListMedia.Data[i] = wi;
			}
		}	
	}
	ListMedia.ItemIndex = -1;
}

bool CBrMedia::Show() {
	Showing = true;

	if (Mode == CBR_MEDIA) {
		ListDirFiles(player()->lpConfig->cf.sMediaPath);
		player()->lpWndBrowser->lpIconMode=ICO_LISTS;
		return player()->lpWndBrowser->Show(_str(STR_MEDIABROWSER), _str(STR_OPTIONS), _str(STR_PLAYLIST), &ListMedia, CBrMediaMsgProc);
	} else {
		Mode = CBR_PLAYLIST;
		wcscpy(ListPlaylist.Title, _str(STR_PLAYLIST));
		ListPlaylistItems();
		player()->lpWndBrowser->lpIconMode=ICO_SETFOLDER;
		return player()->lpWndBrowser->Show(_str(STR_PLAYLISTEDITOR), _str(STR_OPTIONS), _str(STR_MEDIA), &ListPlaylist, CBrMediaMsgProc);
	}
}

void CBrMedia::DoAction(int ac) {

	if (((ac == 0) || (ac == 1)) && (ListMedia.ItemIndex >= 0)) {
		int bCnt = player()->lpPlaylist->Count;
		if (ListMedia.Data[ListMedia.ItemIndex].IconIndex == ICO_DIRECTORY) {
			wchar_t p[MAX_PATH];
			wsprintf(p, L"%s%s\\", player()->lpConfig->cf.sMediaPath, ListMedia.Data[ListMedia.ItemIndex].Text);
			if (player()->lpConfig->cf.AddSubdirs) {
				player()->lpPlaylist->AppendDirectoryRecursive(p);
			} else {
				player()->lpPlaylist->AppendDirectory(p);
			}
		} else if (ListMedia.Data[ListMedia.ItemIndex].IconIndex == ICO_SSONGFILE) {
			wchar_t p[MAX_PATH];
			wsprintf(p, L"%s%s", player()->lpConfig->cf.sMediaPath, ListMedia.Data[ListMedia.ItemIndex].Text);
			player()->lpPlaylist->AppendFile(p);
		}
		if (player()->lpPlaylist->Count > bCnt) {
			if (ac == 0 || ((ac == 1) && (bCnt == 0))) {
				player()->ChangePlaylistIndex(bCnt, false);
				player()->Play();
			}
			player()->lpWndBrowser->Close();
		}
		player()->lpPlaylist->RebuildShuffle();
	}

	if ((ac == 2) || (ac == 3)) {
		if (ac == 2) {
			player()->lpPlaylist->Clear();
		}
		int bCnt = player()->lpPlaylist->Count;
		if (player()->lpConfig->cf.AddSubdirs) {
			player()->lpPlaylist->AppendDirectoryRecursive(player()->lpConfig->cf.sMediaPath);
		} else {
			player()->lpPlaylist->AppendDirectory(player()->lpConfig->cf.sMediaPath);
		}

		if ((player()->lpPlaylist->Count > bCnt)) {
			if ((ac == 2) || (bCnt==0)) {
				if ((ListMedia.ItemIndex >= 0) && (ListMedia.Data[ListMedia.ItemIndex].IconIndex == ICO_SSONGFILE)) {
					player()->ChangePlaylistIndex(bCnt+ListMedia.ItemIndex-DirCount, false);
				} else {
					player()->ChangePlaylistIndex(bCnt, false);
				}
				player()->Play();
			}
		}
		player()->lpPlaylist->RebuildShuffle();
		player()->lpWndBrowser->Close();
	}
}

void CBrMedia::BackToPlaylistMode() {

	Mode = CBR_PLAYLIST;
	player()->lpWndBrowser->SetSoftkey(1, _str(STR_MEDIA));
	wcscpy(ListPlaylist.Title, _str(STR_PLAYLIST));
	player()->lpWndBrowser->lpIconMode=ICO_SETFOLDER;

}

void CBrMedia::UpdatePlaylist() {
	
	if (Showing && (Mode != CBR_MEDIA)) {
		
		int index = ListPlaylist.ItemIndex;
		ListPlaylistItems();
		ListPlaylist.ItemIndex = index;
		player()->lpWndBrowser->Bound();
		player()->lpWndBrowser->Repaint();

	}
}

int CBrMedia::MsgProc(int uMsg) {

	if (uMsg == WBR_DESTROY) {
		Showing = false;
	}

	if (uMsg == WBR_OK) {
		player()->lpWndBrowser->Close();
	}

	if (Mode == CBR_MEDIA) {

		if (uMsg == WBR_MENUSELECTED) {

			if (player()->lpWndBrowser->lpMenu->lpItems == &MenuMediaTap) {

				// 0:Append item and play; 1:Enqueue item; 2:Play current folder; 3:Append current folder
				if ((MenuMediaTap.ItemIndex >= 0) && (MenuMediaTap.ItemIndex < 4)) {
					player()->lpWndBrowser->StopMenu();
					DoAction(MenuMediaTap.ItemIndex);
				}
				
				// 4:Recursive search
				if (MenuMediaTap.ItemIndex == 4) {
					player()->lpConfig->cf.AddSubdirs = !player()->lpConfig->cf.AddSubdirs;
					MenuMediaTap.Data[4].Checked = player()->lpConfig->cf.AddSubdirs;
				}

				// 5:Cancel
				if (MenuMediaTap.ItemIndex == 5) {
					player()->lpWndBrowser->StopMenu();
				}

			}

			if (player()->lpWndBrowser->lpMenu->lpItems == &MenuMedia) {

				// 0:Play current folder; 1:Append current folder
				if ((MenuMedia.ItemIndex == 0) || (MenuMedia.ItemIndex == 1)) {
					if (MenuMedia.ItemIndex == 0) {
						player()->lpPlaylist->Clear();
					}
					int bCnt = player()->lpPlaylist->Count;
					if (player()->lpConfig->cf.AddSubdirs) {
						player()->lpPlaylist->AppendDirectoryRecursive(player()->lpConfig->cf.sMediaPath);
					} else {
						player()->lpPlaylist->AppendDirectory(player()->lpConfig->cf.sMediaPath);
					}
					if (player()->lpPlaylist->Count > bCnt) {
						if ((bCnt==0) || (MenuMedia.ItemIndex == 0)) {
							player()->ChangePlaylistIndex(bCnt, false);
							player()->Play();
						}
					}
					player()->lpPlaylist->RebuildShuffle();
					player()->lpWndBrowser->Close();
				}

				// 2:Play selection; 3:Append selection
				if ((MenuMedia.ItemIndex == 2) || (MenuMedia.ItemIndex == 3)) {
					if (MenuMedia.ItemIndex == 2) {
						player()->lpPlaylist->Clear();
					}
					int bCnt = player()->lpPlaylist->Count;
					for (int i = 0; i < ListMedia.Count; i++) {
						if ((ListMedia.Data[i].Checked) && (ListMedia.Data[i].IconIndex == ICO_DIRECTORY)) {
							wchar_t s[MAX_PATH];
							wsprintf(s, L"%s%s\\", player()->lpConfig->cf.sMediaPath, ListMedia.Data[i].Text);
							if (player()->lpConfig->cf.AddSubdirs) {
								player()->lpPlaylist->AppendDirectoryRecursive(s);
							} else {
								player()->lpPlaylist->AppendDirectory(s);
							}
						} else if ((ListMedia.Data[i].Checked) && (ListMedia.Data[i].IconIndex == ICO_SSONGFILE)) {
							wchar_t s[MAX_PATH];
							wsprintf(s, L"%s%s", player()->lpConfig->cf.sMediaPath, ListMedia.Data[i].Text);
							player()->lpPlaylist->AppendFile(s);
						}
					}
					if (player()->lpPlaylist->Count > bCnt) {
						if ((bCnt==0) || (MenuMedia.ItemIndex == 2)) {
							player()->ChangePlaylistIndex(bCnt, false);
							player()->Play();
						}
					}
					player()->lpPlaylist->RebuildShuffle();
					player()->lpWndBrowser->Close();
				}

				// 4:Recursive search
				if (MenuMedia.ItemIndex == 4) {
					player()->lpConfig->cf.AddSubdirs = !player()->lpConfig->cf.AddSubdirs;
					MenuMedia.Data[4].Checked = player()->lpConfig->cf.AddSubdirs;
				}

				// 5:Select all; 6:Select none; 7: Invert selection
				if ((MenuMedia.ItemIndex == 5) || (MenuMedia.ItemIndex == 6) || (MenuMedia.ItemIndex == 7)) {
					for (int i = 0; i < ListMedia.Count; i++) {
						switch (MenuMedia.ItemIndex) {
							case 5: ListMedia.Data[i].Checked = true; break;
							case 6: ListMedia.Data[i].Checked = false; break;
							case 7: ListMedia.Data[i].Checked = !ListMedia.Data[i].Checked; break;
						}
					}
					player()->lpWndBrowser->StopMenu();
				}

				//8:Back to the player
				if (MenuMedia.ItemIndex == 8) {
					player()->lpWndBrowser->Close();
				}
			}
		}
		
		if (uMsg == WBR_LSOFTKEY) {
			MenuMedia.Data[4].Checked = player()->lpConfig->cf.AddSubdirs;
			player()->lpWndBrowser->StartMenu(&MenuMedia);
		}

		if (uMsg == WBR_RSOFTKEY) {
			Mode = CBR_PLAYLIST;
			player()->lpWndBrowser->lpIconMode=ICO_SETFOLDER;
			player()->lpWndBrowser->SetSoftkey(1, _str(STR_MEDIA));
			player()->lpWndBrowser->SetTitle(_str(STR_PLAYLISTEDITOR));
			ListPlaylistItems();
			player()->lpWndBrowser->ChangeList(&ListPlaylist);
		}

		if (uMsg == WBR_LONGTAP) {
			MenuMediaTap.Data[4].Checked = player()->lpConfig->cf.AddSubdirs;
			player()->lpWndBrowser->StartMenu(&MenuMediaTap);	
		}

		if ((uMsg == WBR_SELECTED) && (ListMedia.ItemIndex >= 0)) {
			if (ListMedia.Data[ListMedia.ItemIndex].IconIndex == ICO_DIRECTORY) {
				wchar_t nw[MAX_PATH];
				wsprintf(nw, L"%s%s\\", player()->lpConfig->cf.sMediaPath, ListMedia.Data[ListMedia.ItemIndex].Text);
				wcscpy(player()->lpConfig->cf.sMediaPath, nw);
				ListMedia.OffY = 0;
				ListDirFiles(player()->lpConfig->cf.sMediaPath);
			} else if (ListMedia.Data[ListMedia.ItemIndex].IconIndex == ICO_SSONGFILE) {
				DoAction(player()->lpConfig->cf.DefBrowserAction);
			}
		}

		if (uMsg == WBR_CLICKTITLE) {

			wchar_t dirName[MAX_PATH];
			wchar_t newPath[MAX_PATH];
			ExtractDirName(dirName, player()->lpConfig->cf.sMediaPath);
			GetUpDir(newPath, player()->lpConfig->cf.sMediaPath);

			wcscpy(player()->lpConfig->cf.sMediaPath, newPath);

			ListDirFiles(player()->lpConfig->cf.sMediaPath);
			for (int i = 0; i < DirCount; i++) {
				if (strCompare(ListMedia.Data[i].Text, dirName) == 0) {
					ListMedia.ItemIndex = i;
				}
			}
			player()->lpWndBrowser->Constrain();
		}

		return 0;
	}

	if (Mode == CBR_PLAYLIST) {

		if (uMsg == WBR_MENUSELECTED) {
		
			if (player()->lpWndBrowser->lpMenu->lpItems == &MenuPlaylist) {
			
				// 0:Delete selected
				if (MenuPlaylist.ItemIndex == 0) {

					int oldIndex = ListPlaylist.ItemIndex;
					player()->lpWndBrowser->StopMenu();
					for (int i = 0; i < ListPlaylist.Count; i++) {
						player()->lpPlaylist->Data[i].DelMark = ListPlaylist.Data[i].Checked;
					}
					player()->lpPlaylist->DeleteMarked();
					player()->lpPlaylist->RebuildShuffle();
					ListPlaylistItems();
					ListPlaylist.ItemIndex = min(oldIndex, ListPlaylist.Count-1);
					player()->lpWndBrowser->Constrain();

				}

				// 1:Clear playlist
				if (MenuPlaylist.ItemIndex == 1) {
					player()->lpWndBrowser->StopMenu();
					player()->lpPlaylist->Clear();
					ListPlaylistItems();
					player()->lpPlaylist->RebuildShuffle();
					player()->lpWndBrowser->Constrain();
				}

				// 2:Go to playing song
				if (MenuPlaylist.ItemIndex == 2) {
					player()->lpWndBrowser->StopMenu();
					if (player()->PlayingIndex > -1) {
						ListPlaylist.ItemIndex = player()->PlayingIndex;
						player()->lpWndBrowser->Constrain();
					}
				}

				// 3:Select all; 4:Select none; 5:Invert selection
				if ((MenuPlaylist.ItemIndex == 3) || (MenuPlaylist.ItemIndex == 4) || (MenuPlaylist.ItemIndex == 5)) {
					player()->lpWndBrowser->StopMenu();
					for (int i = 0; i < ListPlaylist.Count; i++) {
						switch (MenuPlaylist.ItemIndex) {
							case 3: ListPlaylist.Data[i].Checked = true; break;
							case 4: ListPlaylist.Data[i].Checked = false; break;
							case 5: ListPlaylist.Data[i].Checked = !ListPlaylist.Data[i].Checked; break;
						}
					}
				}

				// 6:Back to the player
				if (MenuPlaylist.ItemIndex == 6) {
					player()->lpWndBrowser->Close();
				}
			}

			if (player()->lpWndBrowser->lpMenu->lpItems == &MenuPlaylistTap) {
			
				// 0:Play selected
				if (MenuPlaylistTap.ItemIndex == 0) {
					player()->ChangePlaylistIndex(ListPlaylist.ItemIndex, false);
					player()->Play();
					player()->lpWndBrowser->Close();
				}

				// 1:Set as next
				if (MenuPlaylistTap.ItemIndex == 1) {
					if (player()->lpConfig->cf.ChangeSongButtons) {
						player()->lpWndBrowser->StopMenu();
						MessageBox(player()->lpWndBrowser->hWnd, _str(STR_THISFUNCTIONISNO), _str(STR_ERROR), MB_ICONERROR|MB_OK);
					} else {
						player()->ChangePlaylistIndex(ListPlaylist.ItemIndex, false);
						player()->lpWndBrowser->Close();
					}
				}

				// 2:Delete
				if (MenuPlaylistTap.ItemIndex == 2) {
					player()->lpWndBrowser->StopMenu();
					int oldIndex = ListPlaylist.ItemIndex;
					player()->lpPlaylist->ClearDelList();
					player()->lpPlaylist->Data[ListPlaylist.ItemIndex].DelMark = true;
					player()->lpPlaylist->DeleteMarked();
					player()->lpPlaylist->RebuildShuffle();
					ListPlaylistItems();
					ListPlaylist.ItemIndex = min(oldIndex, ListPlaylist.Count-1);
					player()->lpWndBrowser->Constrain();
				}

				// 3:Move up
				if (MenuPlaylistTap.ItemIndex == 3) {
					player()->lpWndBrowser->StopMenu();
					if (ListPlaylist.ItemIndex > 0) {
						int oldIndex = ListPlaylist.ItemIndex;
						player()->lpPlaylist->Swap(ListPlaylist.ItemIndex, ListPlaylist.ItemIndex-1);
						player()->lpPlaylist->RebuildShuffle();
						ListPlaylistItems();
						ListPlaylist.ItemIndex = oldIndex-1;
						player()->lpWndBrowser->Constrain();
					}
				}

				// 4:Move down
				if (MenuPlaylistTap.ItemIndex == 4) {
					player()->lpWndBrowser->StopMenu();
					if ((ListPlaylist.ItemIndex >= 0) && (ListPlaylist.ItemIndex < ListPlaylist.Count-1)) {
						int oldIndex = ListPlaylist.ItemIndex;
						player()->lpPlaylist->Swap(ListPlaylist.ItemIndex, ListPlaylist.ItemIndex+1);
						player()->lpPlaylist->RebuildShuffle();
						ListPlaylistItems();
						ListPlaylist.ItemIndex = oldIndex+1;
						player()->lpWndBrowser->Constrain();
					}
				}

				// 5:Move to...
				if (MenuPlaylistTap.ItemIndex == 5) {
					player()->lpWndBrowser->StopMenu();
					Mode = CBR_MOVETO;
					MoveToFrom = ListPlaylist.ItemIndex;
					wcscpy(ListPlaylist.Title, _str(STR_PLAYLISTMOVEITEM));
					player()->lpWndBrowser->SetSoftkey(1, _str(STR_CANCEL));
				}

				// 6:Go to folder
				if (MenuPlaylistTap.ItemIndex == 6) {
					player()->lpWndBrowser->StopMenu();
					if (ListPlaylist.ItemIndex >= 0) {
						Mode = CBR_MEDIA;
						player()->lpWndBrowser->SetSoftkey(1, _str(STR_PLAYLIST));
						player()->lpWndBrowser->SetTitle(_str(STR_MEDIABROWSER));
						ExtractFilePath(player()->lpConfig->cf.sMediaPath, player()->lpPlaylist->Data[ListPlaylist.ItemIndex].FileName);
						ListDirFiles(player()->lpConfig->cf.sMediaPath);
						player()->lpWndBrowser->ChangeList(&ListMedia);
						for (int i = 0; i < ListMedia.Count; i++) {
							if (strCompare(ListPlaylist.Data[ListPlaylist.ItemIndex].Text, ListMedia.Data[i].Text) == 0) {
								ListMedia.ItemIndex = i;
							}
						}
						player()->lpWndBrowser->Constrain();
					}
				}

				// 7:Cancel
				if (MenuPlaylistTap.ItemIndex == 7) {
					player()->lpWndBrowser->StopMenu();
				}
			}

		}

		if (uMsg == WBR_LSOFTKEY) {
			player()->lpWndBrowser->StartMenu(&MenuPlaylist);
		}

		if (uMsg == WBR_RSOFTKEY) {
			Mode = CBR_MEDIA;
			player()->lpWndBrowser->lpIconMode=ICO_LISTS;
			player()->lpWndBrowser->SetSoftkey(1, _str(STR_PLAYLIST));
			player()->lpWndBrowser->SetTitle(_str(STR_MEDIABROWSER));
			ListDirFiles(player()->lpConfig->cf.sMediaPath);
			player()->lpWndBrowser->ChangeList(&ListMedia);
		}

		if (uMsg == WBR_LONGTAP) {
			player()->lpWndBrowser->StartMenu(&MenuPlaylistTap);	
		}
	
		if ((uMsg == WBR_SELECTED) && (ListPlaylist.ItemIndex >= 0)) {
			player()->ChangePlaylistIndex(ListPlaylist.ItemIndex, false);
			if (player()->PlayingIndex != ListPlaylist.ItemIndex) {
				player()->Play();
			}
			player()->lpWndBrowser->Close();
		}
	}
	
	if (Mode == CBR_MOVETO) {
		
		if (((uMsg == WBR_SELECTED) || (uMsg == WBR_LONGTAP)) && (ListPlaylist.ItemIndex >= 0)) {
			player()->lpWndBrowser->StartMenu(&MenuMoveToTap);
		}
		
		if (uMsg == WBR_LSOFTKEY) {
			player()->lpWndBrowser->StartMenu(&MenuMoveTo);
		}

		if (uMsg == WBR_RSOFTKEY) {
			BackToPlaylistMode();
		}

		if (uMsg == WBR_MENUSELECTED) {
		
			if (player()->lpWndBrowser->lpMenu->lpItems == &MenuMoveToTap) {
			
				// 0:Befrore this item; 1:After this item
				if ((MenuMoveToTap.ItemIndex == 0) || (MenuMoveToTap.ItemIndex == 1)) {
					player()->lpWndBrowser->StopMenu();
					int newIndex = player()->lpPlaylist->MoveTo(MoveToFrom, ListPlaylist.ItemIndex, (MenuMoveToTap.ItemIndex == 0));
					player()->lpPlaylist->RebuildShuffle();
					ListPlaylistItems();
					ListPlaylist.ItemIndex = newIndex;
					player()->lpWndBrowser->Constrain();
					BackToPlaylistMode();
				}

				// 2:Swap
				if (MenuMoveToTap.ItemIndex == 2) {
					player()->lpWndBrowser->StopMenu();
					int newIndex = ListPlaylist.ItemIndex;
					player()->lpPlaylist->Swap(MoveToFrom, ListPlaylist.ItemIndex);
					player()->lpPlaylist->RebuildShuffle();
					ListPlaylistItems();
					ListPlaylist.ItemIndex = newIndex;
					player()->lpWndBrowser->Constrain();
					BackToPlaylistMode();
				}

				// 3:Cancel move
				if (MenuMoveToTap.ItemIndex == 3) {
					player()->lpWndBrowser->StopMenu();
					BackToPlaylistMode();
				}

			}

			if (player()->lpWndBrowser->lpMenu->lpItems == &MenuMoveTo) {
				
				// 0:Cancel move
				if (MenuMoveTo.ItemIndex == 0) {
					player()->lpWndBrowser->StopMenu();
					BackToPlaylistMode();
				}

			}
		}
	}
	return 0;
}

int CBrMediaMsgProc(int uMsg) {
	return player()->lpBrMedia->MsgProc(uMsg);
}

CBrMedia::~CBrMedia() {

	ClearWbrList(&ListPlaylist);
	ClearWbrList(&ListMedia);

}
