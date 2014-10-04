// Interface types

struct XBITMAP {
	bool Exists;
	int Width, Height;
	HBITMAP hBitmap, hBitmapOld;
	HDC hDC;
};

typedef wchar_t WTEXTSTR[100];

enum IITEMKIND { IIK_BUTTON, IIK_TEXT, IIK_WHEEL, IIK_TRACKBAR, IIK_SPECIALAREA };
enum IBUTTONPROP { IBP_XPOS, IBP_YPOS, IBP_WIDTH, IBP_HEIGHT, IBP_COMMAND };
enum IBUTTONMODE { IBM_NORMAL, IBM_PRESSED };
enum IBUTTONCURPOS { IBC_OUTSIDE, IBC_INSIDE };
enum ITEXTPROP { ITP_XPOS, ITP_YPOS, ITP_WIDTH, ITP_HEIGHT, ITP_TEXTTYPE, ITP_FONTSIZE, ITP_FONTCOLOR, ITP_HORZALIGN, ITP_VERTALIGN, ITP_FONTBOLD };
enum ITEXTTYPE { ITT_SONGNAME, ITT_SONGARTIST, ITT_VOLUME, ITT_ELAPSED, ITT_REMAINING, ITT_SYSTIME, ITT_TOTALTIME, ITT_TRACKNUMBER, ITT_TRACKCOUNT };
enum IWHEELPROP { IWP_XCENTER, IWP_YCENTER, IWP_RADIUS, IWP_LCOMMAND, IWP_RCOMMAND, IWP_CCOMMAND, IWP_UPHALFAC, IWP_LOWHALFAC, IWP_BALLDIST, IWP_BALLCNT, IWP_CENTERRADIUS, IWP_SIDEBTNSIZE };
enum IWHEELMODE { IWM_NORMAL, IWM_WHEELPRESSED, IWM_CPRESSED, IWM_LPRESSED, IWM_RPRESSED };
enum IWHEELCURPOS { IWC_CBUTTON, IWC_WHEEL, IWC_LBUTTON, IWC_RBUTTON, IWC_OUTSIDE };
enum IWHELLACTION { IWA_CHANGEMUSIC, IWA_CHANGEVOLUME, IWA_SEEK };
enum ITRACKBARPROP { IRP_XPOS, IRP_YPOS, IRP_WIDTH, IRP_HEIGHT, IRP_SHOWSCROLLER, IRP_TRACKTYPE };
enum ITRACKBARTYPE { IRT_TIME, IRT_VOLUME };
enum ITRACKBARCURPOS { IRC_OUTSIDE, IRC_INSIDE };
enum ISPECIALAREAPROP { ISP_XPOS, ISP_YPOS, ISP_WIDTH, ISP_HEIGHT, ISP_AREATYPE };
enum ICOMMAND { ICMD_VOLUMEUP, ICMD_VOLUMEDOWN, ICMD_SETTINGS, ICMD_PLAYLIST, ICMD_PREVSONG, ICMD_NEXTSONG, ICMD_PLAYPAUSE, ICMD_CLOSE, ICMD_BLANKSCREEN, ICMD_SHUFFLE, ICMD_REPEAT, ICMD_GOTOCURRENT, ICMD_SLEEPMODE, ICMD_EQUALIZER, ICMD_MINIMIZE };
enum IBROWSERCOLOR { IBC_ITEMSBG, IBC_ITEMSTEXT, IBC_TITLEBG, IBC_TITLETEXT, IBC_SELITEMBG, IBC_SELITEMTEXT, IBC_LINE, IBC_SCROLLER, IBC_RIGHTSIDE, IBC_CHECKED, IBC_SCROLLERBG };
enum IBROWSERPROP { IBP_TITLEHEIGHT, IBP_TITLEFONTSIZE, IBP_TITLEFONTBOLD, IBP_ITEMHEIGHT, IBP_ITEMFONTSIZE, IBP_ITEMFONTBOLD, IBP_SCROLLERSIZE };
enum ISPECIALAREATYPE { IST_ALBUMART };

// iWheel bitmaps
#define IWB_WHEEL		0
#define IWB_CENTER		2
#define IWB_LEFTBTN		6
#define IWB_RIGHTBTN	10
#define IWB_BALL		14

// iWheel bitmap modifiers
#define IWB_NORMAL		0
#define IWB_PRESSED		1
#define IWB_TOGNORMAL	2
#define	IWB_TOGPRESSED	3

struct IITEM {
	IITEMKIND Kind;
	int * IntProp;
	int IntPropCount;
	XBITMAP * BmpProp;
	int BmpPropCount;
	int CustomProp;
	double CustomProp1;
};

struct IITEMS {
	IITEM * Data;
	int Count;
};

//enum IPLAYLISTCOLOR { IPLS_CBG, IPLS_CDIRBG, IPLS_CDIRTXT, IPLS_CITMTXT, IPLS_CSELITMBG, IPLS_CSELITMTXT, IPLS_CLINE };

struct IBROWSER {
	int * IntProp;
    int * Color;
	int ColorCount;
	int IntPropCount;
};

struct IICONS {
	int IconSize;
	int Count;
	XBITMAP * Data;
};


//enum IWHEELAC { IWHL_ACHANGEMUSIC, IWHL_ACHANGEVOLUME, IWHL_ASEEK };

// Interface functions
int get_devWid();
void StartInterface();
int get_devHei();
IITEM * getItem(int Index);
int getItemsCount();
COLORREF getBrowserColor(int Index);
int getBrowserProp(int Index);
double iGetTrackPosition(ITRACKBARTYPE tType);
XBITMAP * getIcon(int Index);
IICONS * getIcons();
void iGetText(LPWSTR lpOut, ITEXTTYPE TextType);

void iSetText(ITEXTTYPE tType, LPWSTR newText);
void iSetTrackPosition(ITRACKBARTYPE tType, double newPosition);
void iSetCmdToggle(ICOMMAND Cmd, bool newToggle);

void iDrawTexts(HDC hDC, LPRECT area, int alsoType);
void iDrawText(HDC hDC, int Index, int offX, int offY);
void iDrawTextBg(HDC hDC, int Index, int offX, int offY);
void iDrawWheel(HDC hDC, int Index, int offX, int offY);
void iDrawButton(HDC hDC, int Index, int offX, int offY);
void iDrawTrackbar(HDC hDC, int Index, bool offScreen);
void iDrawFull(HDC hDC);
void iDrawAlbumArt(HDC hDC);

bool iLoadSkin(HWND hWnd, LPWSTR fileName, bool quiet);

IWHEELCURPOS iWheelCurPos(int Index, int pX, int pY);
IBUTTONCURPOS iButtonCurPos(int Index, int pX, int pY);
ITRACKBARCURPOS iTrackbarCurPos(int Index, int pX, int pY);

double iCursorPosToTrackPosition(int Index, int pX);

void iDrawCheckbox(HDC hDC, int x, int y, int width, int height, LPWSTR text, bool checked);
bool iPointInRect(int pX, int pY, LPRECT rct);