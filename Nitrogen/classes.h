#pragma once
#include "includes\libmad.h"
#include "includes\mapplugin.h"
#include "includes\libovd.h"
#include "includes\maplay.h"
#include "jpeglib\jpeglib.h"
#include "interface.h"

#if _WIN32_WCE >= 0x500
#include <regext.h>
#include <snapi.h>
#endif

enum CBRMODE { CBR_MEDIA, CBR_PLAYLIST, CBR_MOVETO };

// Structures for blanking screen

#define QUERYESCSUPPORT		8 
#define GETVFRAMEPHYSICAL	6144
#define GETVFRAMELEN		6145
#define DBGDRIVERSTAT		6146
#define SETPOWERMANAGEMENT	6147
#define GETPOWERMANAGEMENT	6148


typedef enum _VIDEO_POWER_STATE {
    VideoPowerOn = 1,
    VideoPowerStandBy,
    VideoPowerSuspend,
    VideoPowerOff
} VIDEO_POWER_STATE, *PVIDEO_POWER_STATE;

typedef struct _VIDEO_POWER_MANAGEMENT {
    ULONG Length;
    ULONG DPMSVersion;
    ULONG PowerState;
} VIDEO_POWER_MANAGEMENT, *PVIDEO_POWER_MANAGEMENT;


enum LSOFTKEYACTION { SK_EXIT, SK_MINIMIZE, SK_BLANK, SK_LIBRARY };

struct WBRITEM {
	int IconIndex;
	wchar_t Text[100];
	int TextLength;
	bool Checked;
	bool Separator, Arrow;
};

struct WBRLIST {
	WBRITEM * Data;
	int ItemIndex;
	int Count;
	int TitleIcon;
	int OffY;
	wchar_t Title[MAX_PATH];
	bool CheckEnabled;
};

enum SLEEPACTION {STOPMUSIC, CLOSEAPP, STANDBY };

struct CONFIG {
	bool LowThreadPriority;
	SLEEPACTION SleepAction;
	bool Id3Tags;
	int OutputPrebuffer;
	int OutputBufferLen;
	LSOFTKEYACTION LeftSoftkeyAction;
	bool ForceCleartype;
	bool LockHardKeys;
	bool ChangeSongButtons;
	int Volume;
	EQUALIZER Equalizer;
	int FileAssNotRunning;
	int FileAssRunning;
	bool hapEnabled;
	bool hapDraftQuality;
	int hapRefreshInterval;
	int hapInitialSpeed;
	int hapDeacceleration;
	int hapMinGesture;
	bool AddSubdirs;
	int DefBrowserAction;
	bool Shuffle;
	bool Repeat;
	bool RememberPos;
	bool WriteSongRegistry;
	bool useSystemVolume;
	wchar_t sMediaPath[MAX_PATH];
	wchar_t sSkinFile[100];
	wchar_t sLangFile[100];
};

struct EQPRESET {
	wchar_t name[30];
	EQUALIZER eq;
};

class CConfig {
	public:
		CONFIG cf;
		bool Defaults;
		bool SaveToFile(LPWSTR cFileName);
		bool LoadFromFile(LPWSTR cFileName);
		void LoadDefaults();
		void ApplyMAPSettings();
		~CConfig();
};

typedef int (*WBRPROC)(int);
typedef int (*MENUPROC)(int);

class CMenu {
	private:
		XBITMAP menuBG;
		RECT menuRect;
		HWND Owner;
		int PY;
		int OldPY;
		bool Dragging;
		int ClickMode;
		int OldOffY;
		void Constrain();
	public:
		CMenu(HWND owner, MENUPROC msgproc);	
		~CMenu();
		WBRLIST * lpItems;

		MENUPROC MsgProc;
		void UpdateMenuSize();
		void Start(WBRLIST * pItems);
		void OnMouseDown(int px, int py);
		void OnMouseMove(int px, int py);
		void OnMouseUp(int px, int py);

		void OnKeyDown(bool firstPress, int nKey);
		void OnKeyUp(int nKey);
		void OnPaint(HDC hDC);
};

class CBrMedia {

	public:
		void UpdatePlaylist();
		bool Showing;
		CBrMedia();
		~CBrMedia();
		WBRLIST ListMedia, ListPlaylist;
		int DirCount;
		int PlsCount;
		bool Show();
		void ListPlaylistItems();
		void ListDirFiles(LPWSTR path);
		int MsgProc(int uMsg);
		void StartMenuNames();
	private:
		void BackToPlaylistMode();
		int MoveToFrom;
		CBRMODE Mode;
		void DoAction(int ac);
		WBRLIST MenuMediaTap, MenuPlaylistTap, MenuMoveToTap;
		WBRLIST MenuMedia, MenuPlaylist, MenuMoveTo;
};

class CWndEqualizer {

	private:
		WBRLIST Menu;
		WBRITEM MenuClose, MenuPresetAdd, MenuPresetDel, MenuPresetClr;
		EQPRESET * preset;
		int presetCount;
		EQUALIZER eq;
		RECT hBoxRect;
		int eqClick;
		int fHeight;
		CMenu * lpMenu;
		int selBand;
		bool ShowingMenu;
		void OnCreate();
		void OnMouseDown(int px, int py);
		void OnMouseMove(int px, int py);
		void OnMouseUp(int px, int py);
		void OnPaint(HDC hDC);
		void OnKeyPress(int nKey);
		void StartMenu();
		void StopMenu();
		void Close();
		wchar_t AddPresetName[30];
		void CreateMenuBar(LPWSTR lText, LPWSTR rText);

	public:
		CWndEqualizer();
		~CWndEqualizer();
		ATOM InitClass();
		bool Show();
		HWND hWnd;
		HWND dhWnd;
		HWND hWndMenuBar;
		int MenuProc(int uMsg);
		LRESULT DlgAddPresetProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

};

typedef wchar_t WSOFTKEYSTR[20];

class CWndBrowser {
	public:
		CWndBrowser();
		~CWndBrowser();
		WBRPROC MsgProc;
		ATOM InitClass();
		bool CWndBrowser::Show(LPWSTR lpTitle, LPWSTR lSoft, LPWSTR rSoft, WBRLIST * List, WBRPROC msgproc);
		void Close();
		HWND hWnd;
		HWND hWndMenuBar;
		CMenu * lpMenu;
		WBRLIST * lpList;
		int lpIconMode;
		void StopMenu();
		int MenuProc(int uMsg);
		void StartMenu(WBRLIST * lMenu);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void SetSoftkey(int nSoft, LPWSTR newText);
		void SetTitle(LPWSTR newTitle);
		void ChangeList(WBRLIST * newList);
		void Repaint();
		void Bound();
		void Constrain();
	private:
		bool saUp;
		bool longTap;
		WSOFTKEYSTR lpSoft[2];
		double hapAcc;
		double hapFPY;
		bool hapDir;
		bool hapEnabled;
		DWORD hapTick;
		int hapYCord;
		bool disableClick;
		int ClickMode;
		bool dragging;
		int oldOffY;
		int oldPY;
		int pY;

		bool keyProc;

		bool ShowingMenu;
		void OnMouseDown(int px, int py);
		void OnMouseMove(int px, int py);
		void OnMouseUp(int px, int py);
		void OnKeyDown(bool firstPress, int nKey);
		void OnKeyUp(int nKey);
		void OnTimer(int tmrId);
		void OnCreate();
		void OnPaint(HDC hDC);
		void CreateMenuBar(LPWSTR lText, LPWSTR rText);

};

class CWndAbout {
	public:
		CWndAbout();
		~CWndAbout();
		bool Show();
		HWND hWnd;
		HWND hWndMenuBar;
		ATOM InitClass();
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		XBITMAP bg;
		void OnCreate();
		void OnPaint(HDC hDC);
};

class CWndMain {
	public:
		CWndMain();
		~CWndMain();
		HWND hWnd;
		HWND hWndMenuBar;

		bool Showing;
		bool ShowingMenu;
		CMenu * lpMenu;

		ATOM InitClass();
		bool Show(bool minimized);

		WBRLIST MenuMain;
		WBRLIST MenuSettings;

		int MenuProc(int uMsg);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void Repaint(int mode, int cmd);
		void UpdatePlayPos(bool UpdateReg = true);
		void SetLSoftkeyText();
	
		void UpdateAlbumArt();
		void StartMenuNames();

	private:
		bool inCallPlaying;
#if _WIN32_WCE >= 0x500
		HREGNOTIFY hregNotifyPhone;
#endif

		void UpdateSysTime();

		void OnExternalCmd(DWORD uCmd);
		void OnButtonClick(int command);
		void OnButtonSecFnc(int command);
		void StartSecFncTimer(int command);
		void OnMouseDown(int px, int py);
		void OnMouseMove(int px, int py);
		void OnMouseUp(int px, int py);
		void OnSoftkeyClick(int softkey);

		void OnKeyDown(bool firstPress, int nKey);
		void OnKeyUp(int nKey);
		void OnTimer(int tmrId);
		bool OnCreate();
		void OnPaint(HDC hDC);
		void OnCopyData(LPVOID lpData);

		int paintMode;
		bool paintCommand[40];
		
		bool keyPressed[256];

		int ClickIndex;
		int ClickMode;
		int ClickProp;
		int SecFncCmd;
		bool SecFnc;

		double deltaRot;
		double lastCursorRot;
		double oldCursorRot;
		int oldPlaylistIndex;
		int oldVol;
		long oldPos;
		double oldWheelRot;
		int wheelClickHalf;
		int wheelTurns;

		int indRepaint;

		void StartMenu(bool settings);
		void StopMenu();
		void CreateMenuBar(LPWSTR lText, LPWSTR rText);
};

struct PLAYLISTENTRY {
	wchar_t FileName[MAX_PATH];
	int ShuffleIndex;
	bool DelMark;
};

class CBrSkin {

	public:
		CBrSkin();
		~CBrSkin();
		bool Show();
		WBRLIST ListSkins;
		int MsgProc(int uMsg);
};

typedef wchar_t WPATHSTR[50];

class CBrLang {

	public:
		CBrLang();
		~CBrLang();
		bool Show();
		WBRLIST ListLangs;
		WPATHSTR * NameLangs;
		int MsgProc(int uMsg);
};

class CPlaylist {

	private:
		void FreeData();
		void AppendSearch(LPWSTR search);
		void Sort(int start, int end);	
		bool LoadFromFileNPL(LPWSTR cFileName);
		bool LoadFromFilePLS(LPWSTR cFileName);
		bool LoadFromFileM3U(LPWSTR cFileName);
	public:
		void ReadSongDuration();
		wchar_t IndexAlbumArtFilename[MAX_PATH];
		void RebuildShuffle();
		int MoveTo(int ifrom, int ito, bool before);
		void Swap(int i1, int i2);
		void ClearDelList();
		void DeleteMarked();
		void Clear();
		CPlaylist();
		~CPlaylist();
		int Index;
		int ShuffleIndex;
		int Count;
		PLAYLISTENTRY * Data;
		bool SaveToFile(LPWSTR cFileName, bool Playing, int PlayingIndex, DWORD CurrentPos);
		bool LoadFromFile(LPWSTR cFileName);
		void AppendPlaylist(CPlaylist * src);
		int AppendFile(LPWSTR FileName);
		int GetFileIndex(LPWSTR cFileName);
		void AppendDirectory(LPWSTR Path);
		void AppendDirectoryRecursive(LPWSTR Path);
};

class CDlgFileAssoc {

	public:
		bool IsExtAssigned(LPWSTR extName, LPWSTR extDesc);
		bool IsExtAssignedDyn(LPWSTR extName);
		void ExtAssignDyn(LPWSTR extName, LPWSTR extCmd, int iconIndex);
		void ExtUnassign(LPWSTR extName);
		void ExtUnassignDyn(LPWSTR extName);
		HWND hWnd;
		HWND hWndMenuBar;
		LRESULT DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void Execute();

	private:
		bool PrevAssoc[20];

};

class CDlgLists {

	public:
		HWND hWnd;
		HWND hWndMenuBar;
		LRESULT DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void Execute();

	private:
		void UpdateUpdown(int updCode, int edtCode);
};

class CDlgSettings {

	public:
		HWND hWnd;
		HWND hWndMenuBar;
		LRESULT DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void Execute();

	private:
		void UpdateUpdown(int updCode, int edtCode);
};

class CDlgSleep {

	public:
		CDlgSleep();
		int SleepMins;
		bool SleepActive;
		HWND hWnd;
		LRESULT DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void Execute();

	private:
		void UpdateText();

};

typedef wchar_t WEXTSTR[6];

class CNtPlayer {
	
	public:
		CNtPlayer(HINSTANCE hInst, LPWSTR cmdLine);
		~CNtPlayer();
		wchar_t lpCmdLine[MAX_PATH];
		HINSTANCE hInstance;
		wchar_t Path[MAX_PATH];
		HANDLE MAP;

		int musicCode;
		int diskWaitFlag;
		int firstStart;

		CConfig * lpConfig;
		CPlaylist * lpPlaylist;
		CWndMain * lpWndMain;
		CWndBrowser * lpWndBrowser;
		CBrMedia * lpBrMedia;
		CWndEqualizer * lpWndEqualizer;
		CWndAbout * lpWndAbout;
		CDlgSleep * lpDlgSleep;
		CDlgSettings * lpDlgSettings;
		CDlgFileAssoc * lpDlgFileAssoc;
		CDlgLists * lpDlgLists;
		CBrSkin * lpBrSkin;
		CBrLang * lpBrLang;

		wchar_t SkinsPath[MAX_PATH];
		wchar_t FilesPath[MAX_PATH];
		wchar_t LangsPath[MAX_PATH];

		int ExtCount;
		WEXTSTR * ExtNames;

		int ExtPlsCount;
		WEXTSTR * ExtPlsNames;

		bool KeyLockSupport;
		bool KeysLocked;

		int PlayingIndex;
		int PlayingSongDuration;
		int PlayingSongPos;

		void UpdatePrevNextToggle();
		void ChangeVolume(int newVolume, bool relative);
		void ChangePlaylistIndex(int newIndex, bool relative);
		bool Play();
		bool Pause();
		bool StopAndClose();
		void OnChangePlayerStatus(MAP_STATUS newStatus);
		void OnSongEOF();
		void ToggleDisplay(_VIDEO_POWER_STATE PowerState);
		bool IsDisplayOn();

		// S100 platform
		HWND hUIEngineWnd;
		void PostS100Message(UINT Msg, WPARAM wParam, LPARAM lParam);
		void PostS100StatusMessage();
};