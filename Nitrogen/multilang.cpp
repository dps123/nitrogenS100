// multilang.cpp - Multilanguage support for Nitrogen

#include "stdafx.h"
#include "main.h"

LPWSTR langData[LANG_STR_CNT];

LPWSTR langDefs[LANG_STR_CNT] = {L"(language)", L"Media browser", L"Shuffle", L"Repeat", L"Sleep mode", L"Settings", 
        L"About...", L"Blank screen", L"Exit", L"General settings", L"File associations", L"Lists and browsers", 
        L"Equalizer", L"Skin selector", L"Back to previous menu", L"OK", L"Cancel", L"Options", 
        L"Playlist", L"Play current folder", L"Append current folder", L"Play selection", L"Append selection", L"Recursive search", 
        L"Select all", L"Select none", L"Invert selection", L"Back to the player", L"Append item and play", L"Enqueue item", 
        L"Minimize", L"Blank", L"Library", L"The playlist file seems to be broken. Nitrogen will use an empty playlist instead.", L"Could not load the selected skin file. Nitrogen will attempt to load another skin from the 'skins' directory.", L"There are no skin files in the 'skins' directory.", 
        L"None of the files in the 'skins' directory seem to be valid skin files.", L"Error", L"Message", L"Delete selected", L"Clear playlist", L"Go to playing song", 
        L"Set as next", L"Delete", L"Move up", L"Move down", L"Move to...", L"Go to folder", 
        L"Before this item", L"After this item", L"Swap", L"Cancel move", L"Thread priority", L"Low", 
        L"Normal", L"Sleep action", L"Close application", L"Stop music", L"Prebuffer length", L"Output prebuffer", 
        L"Left softkey action", L"Minimize player", L"Force ClearType rendering", L"Lock hardware keys while in standby", L"Change playing song through previous/next buttons", L"Read ID3 tags when available", 
        L"Playlist editor", L"Media", L"Playlist (move item)", L"minutes", L"Active", L"Use Nitrogen to open", 
        L"If the player is not running", L"If the player is already running", L"Add file to playlist", L"Set file as playlist", L"Set folder as playlist", L"Enable kinetic scrolling", 
        L"Kinetic scrolling settings", L"Refresh interval", L"Initial speed", L"Deacceleration", L"Minimal gesture", L"Lower font quality when scrolling", 
        L"Graphic equalizer", L"Enable equalizer", L"Presets", L"Add as preset", L"Remove preset", L"Reset equalizer", 
        L"Enter the name of the preset", L"Please donate if you like this software!", L"Many thanks to everyone who", L"helped make this program usable,", L"stable and good-looking!", L"English original version", 
		L"by Filipe Vilela Soares", L"The version of the skin file is not compatible with the application.", L"The skin file was not found in the directory of the application.", L"Language selector", L"Remember last song position", L"Your device has no key lock support.",
		L"Write current song info on registry", L"This function is not allowed when the 'Previous/next buttons change playing song' setting is on.", L"(empty playlist)", L"(unknown artist)",
		L"Stand by", L"Use system volume", L"Disc waiting ..."};

void lang_loaddefaults() {

	for (int i = 0; i < LANG_STR_CNT; i++) {
		if (langData[i] != NULL) {
			delete [] langData[i];
		}
		langData[i] = strCopy(langDefs[i]);
	}

}

bool lang_loadfromfile(LPWSTR lpFileName) {

	for (int i = 0; i < LANG_STR_CNT; i++) {
		if (langData[i] != NULL) {
			delete [] langData[i];
			langData[i] = NULL;
		}
	}

	bool result = false;
	HANDLE f = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (f != INVALID_HANDLE_VALUE) {
	
		char buf[5];
		buf[4] = 0;

		DWORD n;
	
		if (ReadFile(f, &buf, 4, &n, NULL)) {
		
			if (strcmp("NLNG", buf) == 0) {			

				int cnt = 0;
				if (ReadFile(f, &cnt, 4, &n, NULL)) {
				
					result = true;
					
					for (int i = 0; i < min(cnt, LANG_STR_CNT); i++) {
					
						int len = 0;
						wchar_t sbuf[260];
						if (ReadFile(f, &len, 2, &n, NULL) && len < 260 && ReadFile(f, sbuf, len*sizeof(wchar_t), &n, NULL)) {
							sbuf[len] = 0;
							langData[i] = strCopy(sbuf);
						} else {
							result = false;
							break;
						}
					}
				}

			}

		}
		CloseHandle(f);
	}

	for (int i = 0; i < LANG_STR_CNT; i++) {
		if (langData[i] == NULL) {
			langData[i] = strCopy(langDefs[i]);
		}
	}
	
	return result;
}

LPWSTR _str(DWORD index) {

	return langData[index];

}

void lang_init() {
	
	for (int i = 0; i < LANG_STR_CNT; i++) {
		langData[i] = NULL;
	}

}

void lang_destroy() {

	for (int i = 0; i < LANG_STR_CNT; i++) {
		if (langData[i] != NULL) {
			delete [] langData[i];
			langData[i] = NULL;
		}
	}

}

bool lang_getname(LPWSTR lpFileName, LPWSTR lpNameOut) {

	bool result = false;
	HANDLE f = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (f != INVALID_HANDLE_VALUE) {
	
		char buf[5];
		buf[4] = 0;

		DWORD n;
	
		if (ReadFile(f, &buf, 4, &n, NULL)) {
		
			if (strcmp("NLNG", buf) == 0) {			

				int cnt = 0;
				if (ReadFile(f, &cnt, 4, &n, NULL) & (cnt > 0)) {
				
					int len = 0;
					wchar_t sbuf[260];
					
					if (ReadFile(f, &len, 2, &n, NULL) && len < 260 && ReadFile(f, sbuf, len*sizeof(wchar_t), &n, NULL)) {
						sbuf[len] = 0;
						wcscpy(lpNameOut, sbuf);
						result = true;
					}
				}
			}
		}
		CloseHandle(f);
	}
	return result;
}
