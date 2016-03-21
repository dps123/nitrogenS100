<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: libFLAC_static - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\tmp\RSP15.tmp" with contents
[
/nologo /W3 /Od /I "." /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "_LIB" /D "_MSC_VER" /D "HAVE_CONFIG_H" /D "FLAC__NO_DLL" /FR"ARMV4Rel/" /Fp"ARMV4Rel/libFLAC_static.pch" /YX /Fo"ARMV4Rel/" /MC /c 
"H:\build\nitrogen\flac\src\libFLAC\bitbuffer.c"
"H:\build\nitrogen\flac\src\libFLAC\bitmath.c"
"H:\build\nitrogen\flac\src\libFLAC\cpu.c"
"H:\build\nitrogen\flac\src\libFLAC\crc.c"
"H:\build\nitrogen\flac\src\libFLAC\file_decoder.c"
"H:\build\nitrogen\flac\src\libFLAC\file_encoder.c"
"H:\build\nitrogen\flac\src\libFLAC\fixed.c"
"H:\build\nitrogen\flac\src\libFLAC\float.c"
"H:\build\nitrogen\flac\src\libFLAC\format.c"
"H:\build\nitrogen\flac\src\libFLAC\lpc.c"
"H:\build\nitrogen\flac\src\libFLAC\md5.c"
"H:\build\nitrogen\flac\src\libFLAC\memory.c"
"H:\build\nitrogen\flac\src\libFLAC\metadata_object.c"
"H:\build\nitrogen\flac\src\libFLAC\seekable_stream_decoder.c"
"H:\build\nitrogen\flac\src\libFLAC\seekable_stream_encoder.c"
"H:\build\nitrogen\flac\src\libFLAC\stream_decoder.c"
"H:\build\nitrogen\flac\src\libFLAC\stream_encoder.c"
"H:\build\nitrogen\flac\src\libFLAC\stream_encoder_framing.c"
]
Creating command line "clarm.exe @c:\tmp\RSP15.tmp" 
Creating temporary file "c:\tmp\RSP16.tmp" with contents
[
/nologo /out:"ARMV4Rel\libFLAC_static.lib" 
.\ARMV4Rel\bitbuffer.obj
.\ARMV4Rel\bitmath.obj
.\ARMV4Rel\cpu.obj
.\ARMV4Rel\crc.obj
.\ARMV4Rel\file_decoder.obj
.\ARMV4Rel\file_encoder.obj
.\ARMV4Rel\fixed.obj
.\ARMV4Rel\float.obj
.\ARMV4Rel\format.obj
.\ARMV4Rel\lpc.obj
.\ARMV4Rel\md5.obj
.\ARMV4Rel\memory.obj
.\ARMV4Rel\metadata_object.obj
.\ARMV4Rel\seekable_stream_decoder.obj
.\ARMV4Rel\seekable_stream_encoder.obj
.\ARMV4Rel\stream_decoder.obj
.\ARMV4Rel\stream_encoder.obj
.\ARMV4Rel\stream_encoder_framing.obj
]
Creating command line "link.exe -lib @c:\tmp\RSP16.tmp"
<h3>Output Window</h3>
Compiling...
bitbuffer.c
bitmath.c
cpu.c
crc.c
file_decoder.c
file_encoder.c
fixed.c
float.c
format.c
lpc.c
md5.c
memory.c
metadata_object.c
h:\build\nitrogen\flac\src\libflac\metadata_object.c(1186) : warning C4013: 'strnicmp' undefined; assuming extern returning int
seekable_stream_decoder.c
seekable_stream_encoder.c
stream_decoder.c
stream_encoder.c
stream_encoder_framing.c
Creating library...

<h3>
--------------------Configuration: libFLACpp_static - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\tmp\RSP17.tmp" with contents
[
/nologo /W3 /Od /I "." /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "_LIB" /D "_MSC_VER" /D "HAVE_CONFIG_H" /D "FLAC__NO_DLL" /FR"ARMV4Rel/" /Fp"ARMV4Rel/libFLACpp_static.pch" /YX /Fo"ARMV4Rel/" /MC /c 
"H:\build\nitrogen\flac\SRC\LIBFLAC++\file_decoder.cpp"
"H:\build\nitrogen\flac\SRC\LIBFLAC++\file_encoder.cpp"
"H:\build\nitrogen\flac\SRC\LIBFLAC++\seekable_stream_decoder.cpp"
"H:\build\nitrogen\flac\SRC\LIBFLAC++\seekable_stream_encoder.cpp"
"H:\build\nitrogen\flac\SRC\LIBFLAC++\stream_decoder.cpp"
"H:\build\nitrogen\flac\SRC\LIBFLAC++\stream_encoder.cpp"
]
Creating command line "clarm.exe @c:\tmp\RSP17.tmp" 
Creating temporary file "c:\tmp\RSP18.tmp" with contents
[
/nologo /out:"ARMV4Rel\libFLACpp_static.lib" 
".\ARMV4Rel\file_decoder.obj"
".\ARMV4Rel\file_encoder.obj"
".\ARMV4Rel\seekable_stream_decoder.obj"
".\ARMV4Rel\seekable_stream_encoder.obj"
".\ARMV4Rel\stream_decoder.obj"
".\ARMV4Rel\stream_encoder.obj"
]
Creating command line "link.exe -lib @c:\tmp\RSP18.tmp"
<h3>Output Window</h3>
Compiling...
file_decoder.cpp
file_encoder.cpp
seekable_stream_decoder.cpp
seekable_stream_encoder.cpp
stream_decoder.cpp
stream_encoder.cpp
Creating library...

<h3>
--------------------Configuration: gspflac - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\tmp\RSP19.tmp" with contents
[
/nologo /W3 /Od /I "../../include" /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_USRDLL" /D "GSPFLAC_EXPORTS" /FR"ARMV4Rel/" /Fp"ARMV4Rel/gspflac.pch" /YX /Fo"ARMV4Rel/" /MC /c 
"H:\build\nitrogen\flac\src\plugin_gsplayer\dither.c"
"H:\build\nitrogen\flac\src\plugin_gsplayer\filedecoder.cpp"
"H:\build\nitrogen\flac\src\plugin_gsplayer\gspflac.cpp"
]
Creating command line "clarm.exe @c:\tmp\RSP19.tmp" 
Creating temporary file "c:\tmp\RSP1A.tmp" with contents
[
commctrl.lib coredll.lib libflac_static.lib libflacpp_static.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:no /pdb:"ARMV4Rel/gspflac.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4Rel/gspflac.dll" /implib:"ARMV4Rel/gspflac.lib" /libpath:"../libFLAC/ARMV4/ARMV4Rel" /libpath:"../libFLAC++/ARMV4/ARMV4Rel" /subsystem:windowsce,4.00 /align:"4096" /MACHINE:ARM 
.\ARMV4Rel\dither.obj
.\ARMV4Rel\filedecoder.obj
.\ARMV4Rel\gspflac.obj
\build\nitrogen\flac\src\libFLAC\ARMV4\ARMV4Rel\libFLAC_static.lib
"\build\nitrogen\flac\SRC\LIBFLAC++\ARMV4\ARMV4Rel\libFLACpp_static.lib"
]
Creating command line "link.exe @c:\tmp\RSP1A.tmp"
<h3>Output Window</h3>
Compiling...
dither.c
filedecoder.cpp
gspflac.cpp
h:\build\nitrogen\flac\src\plugin_gsplayer\gspflac.cpp(144) : warning C4244: 'return' : conversion from 'unsigned __int64' to 'long', possible loss of data
Linking...
   Creating library ARMV4Rel/gspflac.lib and object ARMV4Rel/gspflac.exp




<h3>Results</h3>
gspflac.dll - 0 error(s), 2 warning(s)
</pre>
</body>
</html>
