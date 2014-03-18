#include "resource.h"
#include "classes.h"
#include "multilang.h"
#include <windows.h>

// Definitions of constants

#define DISKWAIT_TIMEOUT    15

#define PI					3.141592653

#define CLK_NONE			0
#define CLK_WHEEL			1
#define CLK_WHEELBTN		2
#define CLK_BUTTON			3
#define CLK_TRACK			4
#define CLK_DRAG			5
#define CLK_TITLE			6
#define CLK_SCROLLER		7
#define CLK_SCROLLAREA		8

#define PNT_FULL			0
#define PNT_WHEEL			1
#define PNT_SONGINFO		2
#define PNT_BUTTON			4
#define PNT_VOLUME			8
#define PNT_TIME			16
#define PNT_TRACK			32
#define PNT_BTCOMMAND		64
#define PNT_SYSTIME			128
#define PNT_PLAYBACK		256
#define PNT_ALBUMART		512
#define PNT_FORCEFULL		1023

#define TMR_UPDATEPOS		1
#define TMR_SLEEP			2
#define TMR_RESETIDLE		3
#define TMR_UPDSYSTIME		4
#define TMR_ALBUMART		5
#define TMR_HAPTICS			6
#define TMR_SECFUNCTION		7
#define TMR_LONGTAP			8
#define TMR_SCROLLAREA		9
#define TMR_LONGKEYPRESS	10
#define TMR_UNLOCKKEYS		11

#define TMR_DISKWAIT		12

#define PREV_CMD			-1

#define WBR_SELECTED		0
#define WBR_LSOFTKEY		1
#define WBR_RSOFTKEY		2
#define WBR_DESTROY			3
#define WBR_CLICKTITLE		4
#define WBR_MENUSELECTED	5
#define WBR_LONGTAP			6
#define WBR_OK				7

#define ECMD_PLAYPAUSE		40001
#define ECMD_PREVSONG		40002
#define ECMD_NEXTSONG		40003
#define ECMD_VOLUMEDOWN		40004
#define ECMD_VOLUMEUP		40005
#define ECMD_PLAY			40006
#define ECMD_PAUSE			40007

#define ICO_DIRECTORY		0
#define ICO_SSONGFILE		1
#define ICO_ARROW			2
#define ICO_MEDIABROWSER	3
#define ICO_EQUALIZER		4
#define ICO_SHUFFLE			5
#define ICO_REPEAT			6
#define ICO_SLEEPMODE		7
#define ICO_SETTINGS		8
#define ICO_ABOUT			9
#define ICO_BLANKSCREEN		10
#define ICO_EXIT			11
#define ICO_ADDSONG			12
#define ICO_ADDFOLDER		13
#define ICO_ADDSELECTION	14
#define ICO_CLOSEMENU		15
#define ICO_PLAYNOW			16
#define ICO_ADDASNEXT		17
//#define ICO_CONTINUESONG	18
#define ICO_BACKTOMENU		19
//#define ICO_DONTSUBDIR	20
//#define ICO_SUBDIR		21
//#define ICO_FOLDEROPT		22
#define ICO_SELECTALL		23
#define ICO_SELECTNONE		24
#define ICO_SELECTINV		25
#define ICO_PLITEM			26
#define ICO_PLITEMPLAY		27
#define ICO_PLITEMNEXT		28
#define ICO_DELETEITEM		29
#define ICO_MOVETO			30
#define ICO_DELCHECKED		31
#define ICO_CLEARPL			32
#define ICO_BEFORETHIS		33
#define ICO_AFTERTHIS		34
#define ICO_SWAP			35
#define ICO_CANCEL			36
#define ICO_SSKINFILE		37
#define ICO_SKINSEL			38
#define ICO_GENERALSET		39
#define ICO_FILEASSOC		40
#define ICO_LISTS			41
#define ICO_SETFOLDER		42
#define ICO_SETSELECTION	43
#define ICO_RECURSIVE		44
#define ICO_GOTOCURRENT		45
#define ICO_MOVEUP			46
#define ICO_MOVEDOWN		47
#define ICO_GOTOFOLDER		48
#define ICO_ADDPRESET		49
#define ICO_REMPRESET		50
#define ICO_RESETEQ			51
#define ICO_PRESET			52
#define ICO_DIRUP			53
#define ICO_LANGSEL			54

#define CURRENT_CONFIG_VERSION	1013
#define SLEEP_INTERVAL			60000

#define WM_NTG_ISPLAYING	WM_USER+3001
#define WM_NTG_GETCURRENT	WM_USER+3002
#define WM_NTG_SETCURRENT	WM_USER+3003
#define WM_NTG_GETVOLUME	WM_USER+3005
#define WM_NTG_SETVOLUME	WM_USER+3006
#define WM_NTG_GETMUSICCODE	WM_USER+3007

#define WM_PHONEINCALL		WM_USER+4001
#define SC_PHONESTATUS		101

#define WM_S100_PLATFORM	33541
#define WM_S100_AUDIO	    33550

// Global Types

enum MPLACTION { MPL_ADDTOPLAYLIST, MPL_SETASPLAYLIST, MPL_SETFOLDER };
enum CMDLINE { CMD_NONE, CMD_SONG, CMD_SKIN };

struct SONGINFO {
	wchar_t songArtist[MAX_TAG_STR];
	wchar_t songName[MAX_TAG_STR];
	wchar_t songAlbum[MAX_TAG_STR];
};

struct NTG_SONGINFO {
	wchar_t songArtist[60];
	wchar_t songTitle[60];
	wchar_t songFileName[260];
};

CNtPlayer * player();
LRESULT CALLBACK CWndMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CWndBrowserProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CWndEqualizerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CDlgAddPresetProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CWndAboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CDlgSleepProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CDlgSettingsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CDlgFileAssocProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CDlgListsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CWndMainMenuProc(int uMsg);
int CBrMediaMsgProc(int uMsg);
int CBrSkinMsgProc(int uMsg);
int CBrLangMsgProc(int uMsg);
int CWndBrowserMenuProc(int uMsg);
int CWndEqualizerMenuProc(int uMsg);

// Functions in utils.cpp
bool		FileExists(LPWSTR search);
bool		GetAlbumArtFilename(LPWSTR lpOut, LPWSTR lpSongFilename, LPWSTR lpSongAlbum);
COLORREF	MixColors(COLORREF c1, COLORREF c2, double grad);
void		ClearWbrList(WBRLIST * wbr);
void		debugint(HWND hWnd, int i);
int			GetFileCount(LPWSTR search);
bool		ByteEqual(LPBYTE b1, LPBYTE b2, int MaxLen);
void		SetSoftkeyText(HWND hWndMenuBar, int softKey, LPWSTR newText);
bool		LoadJPEG(LPWSTR fName, XBITMAP * xbit, int wid, int hei);
void		WndRepaint(HWND hWnd, bool Enable);
void		DeleteXBitmap(XBITMAP * xbmp);
void		CreateXBitmap(XBITMAP * xbmp, int wid, int hei, int bits, bool exists);
bool		CheckFileExt(LPWSTR path, LPWSTR ext);

void		GetSongInfo(SONGINFO * sInfo, LPWSTR songFile);
LPRECT		getRect(int x1, int y1, int x2, int y2);
HFONT		CreateFont(int fSize, int fWeight, bool draft);

int			strCompare(LPWSTR s1, LPWSTR s2);
LPWSTR		strCopy(LPWSTR src);
bool		strExtract(LPWSTR lpOut, LPWSTR src, int ch);
void		SecsToTime(LPWSTR lpOut, int secs);

void		ExtractDirName(LPWSTR lpOut, LPWSTR lpPath);
void		ExtractFilePath(LPWSTR lpOut, LPWSTR lpPath);
void		ExtractFileName(LPWSTR lpOut, LPWSTR lpPath);

bool		FileSearch(LPWSTR lpOut, LPWSTR searchPath, LPWSTR searchFile, LPWSTR searchExt);
void		ChangeFileExt(LPWSTR lpOut, LPWSTR fName, LPWSTR newExt);

bool		ReadFileBitmap(HANDLE f, XBITMAP * xbit);
void		SetWbrItem(WBRITEM * wItem, LPWSTR Text, int IconIndex, bool Checked, bool Separator, bool Arrow);

// Keyboard hook functions and structures

#ifndef _WINCE_KB_HOOK_H 
	#define _WINCE_KB_HOOK_H 
	#define WH_KEYBOARD_LL 20 

	typedef LRESULT (CALLBACK* HOOKPROC)(int code, WPARAM wParam, LPARAM lParam); 
	typedef HHOOK (WINAPI *_SetWindowsHookExW)(int, HOOKPROC, HINSTANCE, DWORD); 
	typedef LRESULT (WINAPI *_CallNextHookEx)(HHOOK, int, WPARAM, LPARAM); 
	typedef LRESULT (WINAPI *_UnhookWindowsHookEx)(HHOOK); 
 
	typedef struct { 
		DWORD vkCode; 
		DWORD scanCode; 
		DWORD flags; 
		DWORD time; 
		ULONG_PTR dwExtraInfo; 
	} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT; 
 
	struct HOOKPROCS {
		_SetWindowsHookExW SetWindowsHookEx; 
		_UnhookWindowsHookEx UnhookWindowsHookEx; 
		_CallNextHookEx CallNextHookEx; 
	};

	// Functions on utils.cpp
	bool HookLoadProcs(HOOKPROCS * hookProcs);
	bool HookCloseProcs(HOOKPROCS * hookProcs);

	// Functions on main.cpp
	LRESULT CALLBACK KBHookProc(int nCode, WPARAM wParam, LPARAM lParam);

	// Functions on cplayer.cpp
	VOID CALLBACK DiskWaitTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

#endif