<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: gspwma - Win32 (WCE ARMV4) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\tmp\RSP42.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /FR"ARMV4Dbg/" /Fp"ARMV4Dbg/gspwma.pch" /YX /Fo"ARMV4Dbg/" /Fd"ARMV4Dbg/" /MC /c 
"H:\BUILD\NITROGEN\GSPWMA\GSPWMA\asfparser.cpp"
"H:\BUILD\NITROGEN\GSPWMA\GSPWMA\gspwma.cpp"
"H:\BUILD\NITROGEN\GSPWMA\GSPWMA\mediabuf.cpp"
"H:\BUILD\NITROGEN\GSPWMA\GSPWMA\wmadmo.cpp"
]
Creating command line "clarm.exe @c:\tmp\RSP42.tmp" 
Creating temporary file "c:\tmp\RSP43.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:yes /pdb:"ARMV4Dbg/gspwma.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4Dbg/gspwma.dll" /implib:"ARMV4Dbg/gspwma.lib" /subsystem:windowsce,4.00 /align:"4096" /MACHINE:ARM 
.\ARMV4Dbg\asfparser.obj
.\ARMV4Dbg\gspwma.obj
.\ARMV4Dbg\mediabuf.obj
.\ARMV4Dbg\wmadmo.obj
.\ARMV4Dbg\gspwma.res
]
Creating command line "link.exe @c:\tmp\RSP43.tmp"
<h3>Output Window</h3>
Compiling...
asfparser.cpp
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(93) : warning C4244: '=' : conversion from 'unsigned __int64' to 'int', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(99) : warning C4244: '=' : conversion from 'unsigned __int64' to 'int', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(157) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(172) : warning C4244: '=' : conversion from 'unsigned __int64' to 'int', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(189) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'unsigned long', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(192) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'unsigned long', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(196) : warning C4244: '-=' : conversion from 'unsigned __int64' to 'unsigned long', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(264) : warning C4244: 'initializing' : conversion from 'unsigned __int64' to 'int', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(269) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'unsigned long', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(272) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'unsigned long', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(346) : warning C4244: 'initializing' : conversion from 'unsigned __int64' to 'int', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(351) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'unsigned long', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(354) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'unsigned long', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(418) : warning C4018: '>' : signed/unsigned mismatch
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(636) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(649) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(576) : warning C4101: 'dwRead' : unreferenced local variable
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(682) : warning C4244: '=' : conversion from 'double' to 'unsigned __int64', possible loss of data
h:\build\nitrogen\gspwma\gspwma\asfparser.cpp(693) : warning C4244: 'return' : conversion from 'double' to 'int', possible loss of data
gspwma.cpp
mediabuf.cpp
wmadmo.cpp
Linking...
   Creating library ARMV4Dbg/gspwma.lib and object ARMV4Dbg/gspwma.exp
gspwma.obj : error LNK2019: unresolved external symbol CoInitializeEx referenced in function "void __cdecl Init(void)" (?Init@@YAXXZ)
gspwma.obj : error LNK2019: unresolved external symbol CoUninitialize referenced in function "void __cdecl Quit(void)" (?Quit@@YAXXZ)
mediabuf.obj : error LNK2001: unresolved external symbol IID_IUnknown
wmadmo.obj : error LNK2019: unresolved external symbol CoTaskMemAlloc referenced in function "long __cdecl InitMediaType(struct _DMOMediaType *,unsigned long)" (?InitMediaType@@YAJPAU_DMOMediaType@@K@Z)
wmadmo.obj : error LNK2019: unresolved external symbol CoTaskMemFree referenced in function "long __cdecl FreeMediaType(struct _DMOMediaType *)" (?FreeMediaType@@YAJPAU_DMOMediaType@@@Z)
ARMV4Dbg/gspwma.dll : fatal error LNK1120: 5 unresolved externals
Error executing link.exe.
Creating command line "bscmake.exe /nologo /o"ARMV4Dbg/gspwma.bsc"  .\ARMV4Dbg\asfparser.sbr .\ARMV4Dbg\gspwma.sbr .\ARMV4Dbg\mediabuf.sbr .\ARMV4Dbg\wmadmo.sbr"
Creating browse info file...
<h3>Output Window</h3>



<h3>Results</h3>
gspwma.dll - 6 error(s), 19 warning(s)
</pre>
</body>
</html>
