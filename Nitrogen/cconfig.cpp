#include "stdafx.h"
#include "main.h"

bool CConfig::SaveToFile(LPWSTR cFileName) {

	HANDLE f = CreateFile(cFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	
	int cver = CURRENT_CONFIG_VERSION;
	DWORD n;
	WriteFile(f, &cver, sizeof(int), &n, false);
	WriteFile(f, &cf, sizeof(CONFIG), &n, false);
	CloseHandle(f);
	return true;
}

bool CConfig::LoadFromFile(LPWSTR cFileName) {

	HANDLE f = CreateFile(cFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	bool result = false;

	if (f != INVALID_HANDLE_VALUE) {
		DWORD n;
		int cver;
		ReadFile(f, &cver, sizeof(int), &n, NULL);
		if (cver == CURRENT_CONFIG_VERSION) {
			ReadFile(f, &cf, sizeof(CONFIG), &n, false);
			result = true;
			Defaults = false;
		}
		CloseHandle(f);	
	}

	return result;
}

void CConfig::LoadDefaults() {

	Defaults = true;

	cf.Id3Tags = true;
	cf.LowThreadPriority = false;
	cf.SleepAction = CLOSEAPP;
	cf.OutputPrebuffer = 50;
	cf.OutputBufferLen = 1500;
	cf.LeftSoftkeyAction = SK_EXIT;
	cf.ForceCleartype = false;
	cf.LockHardKeys = false;
	cf.ChangeSongButtons = false;
	cf.RememberPos = false;
	cf.WriteSongRegistry = true;
	cf.useSystemVolume = true;
	cf.Volume = 100;
	cf.Equalizer.fEnable = false;
	cf.Equalizer.preamp = 31;
	for (int i = 0; i < 10; i++) {
		cf.Equalizer.data[i] = 31;
	}
	cf.FileAssNotRunning = 2;
	cf.FileAssRunning = 0;

	cf.hapEnabled = true;
	cf.hapRefreshInterval = 33;
	cf.hapInitialSpeed = 50;
	cf.hapDeacceleration = 15;
	cf.hapMinGesture = 20;
	cf.hapDraftQuality = true;

	cf.AddSubdirs = false;
	cf.DefBrowserAction = 2;

	cf.Shuffle = false;
	cf.Repeat = false;

	wcscpy(cf.sMediaPath, L"\\");
	wcscpy(cf.sSkinFile, L"default-240-320.sk2");
	memset(cf.sLangFile, 0, sizeof(cf.sLangFile));

}

void CConfig::ApplyMAPSettings() {

	MAP_OPTIONS mop;
	MAP_GetOptions(player()->MAP, &mop);
	mop.nOutputBufferLen = cf.OutputBufferLen;
	mop.nOutputPrebuffer = cf.OutputPrebuffer;
	if (cf.LowThreadPriority) {
		mop.nThreadPriority = THREAD_PRIORITY_BELOW_NORMAL;
	} else {
		mop.nThreadPriority = THREAD_PRIORITY_NORMAL;
	}
	MAP_SetOptions(player()->MAP, &mop);

}

CConfig::~CConfig() {

}