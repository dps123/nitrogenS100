<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: gspmp4 - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating command line "rc.exe /l 0x411 /fo"ARMV4Rel/gspmp4.res" /d UNDER_CE=400 /d _WIN32_WCE=400 /d "NDEBUG" /d "UNICODE" /d "_UNICODE" /d "WCE_PLATFORM_STANDARDSDK" /d "ARM" /d "_ARM_" /d "ARMV4" /r "H:\build\nitrogen\gspmp4\gspmp4\gspmp4\gspmp4.rc"" 
Creating temporary file "c:\tmp\RSP1D.tmp" with contents
[
/nologo /W3 /Oxt /Ob2 /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "UNDER_CE" /Fp"ARMV4Rel/gspmp4.pch" /YX /Fo"ARMV4Rel/" /MC /c 
"H:\build\nitrogen\gspmp4\gspmp4\gspmp4\gspmp4.cpp"
"H:\build\nitrogen\gspmp4\gspmp4\gspmp4\id3tag.cpp"
"H:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp"
"H:\build\nitrogen\gspmp4\gspmp4\gspmp4\utils.c"
]
Creating command line "clarm.exe @c:\tmp\RSP1D.tmp" 
Creating temporary file "c:\tmp\RSP1E.tmp" with contents
[
commctrl.lib coredll.lib ole32.lib uuid.lib ..\lib\ARMV4Rel\libfaad.lib ..\lib\ARMV4Rel\mp4ff.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:no /pdb:"ARMV4Rel/gspmp4.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4Rel/gspmp4.dll" /implib:"ARMV4Rel/gspmp4.lib" /subsystem:windowsce,4.00 /align:"4096" /MACHINE:ARM 
.\ARMV4Rel\gspmp4.obj
.\ARMV4Rel\id3tag.obj
.\ARMV4Rel\in_mp4.obj
.\ARMV4Rel\utils.obj
.\ARMV4Rel\gspmp4.res
]
Creating command line "link.exe @c:\tmp\RSP1E.tmp"
<h3>Output Window</h3>
Compiling resources...
Compiling...
gspmp4.cpp
id3tag.cpp
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\id3tag.cpp(82) : warning C4146: unary minus operator applied to unsigned type, result still unsigned
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\id3tag.cpp(173) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\id3tag.cpp(181) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\id3tag.cpp(196) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\id3tag.cpp(442) : warning C4018: '!=' : signed/unsigned mismatch
in_mp4.cpp
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(146) : warning C4101: 'frame_length' : unreferenced local variable
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(281) : warning C4244: 'argument' : conversion from 'double' to 'long', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(282) : warning C4244: '=' : conversion from 'double' to '__int64', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(272) : warning C4101: 'samplerate' : unreferenced local variable
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(273) : warning C4101: 'channels' : unreferenced local variable
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(620) : warning C4244: 'argument' : conversion from '__int64' to 'long', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(634) : warning C4244: 'argument' : conversion from '__int64' to 'long', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(683) : warning C4244: '=' : conversion from '__int64' to 'int', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(685) : warning C4244: '=' : conversion from '__int64' to 'int', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(531) : warning C4101: 'header' : unreferenced local variable
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(532) : warning C4101: 'hMP4File' : unreferenced local variable
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(528) : warning C4101: 'buffer' : unreferenced local variable
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(529) : warning C4101: 'buffer_size' : unreferenced local variable
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(818) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(862) : warning C4018: '!=' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(866) : warning C4018: '!=' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(871) : warning C4018: '!=' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(891) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(927) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(955) : warning C4018: '!=' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(968) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(1217) : warning C4244: '=' : conversion from 'double' to 'int', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(1269) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(1321) : warning C4018: '<' : signed/unsigned mismatch
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(1257) : warning C4101: 'samplerate' : unreferenced local variable
h:\build\nitrogen\gspmp4\gspmp4\gspmp4\in_mp4.cpp(1258) : warning C4101: 'channels' : unreferenced local variable
utils.c
Linking...
LINK : fatal error LNK1181: cannot open input file '..\lib\ARMV4Rel\libfaad.lib'
Error executing link.exe.



<h3>Results</h3>
gspmp4.dll - 1 error(s), 31 warning(s)
</pre>
</body>
</html>
