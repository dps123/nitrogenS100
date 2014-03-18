<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: libFLAC_static - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\tmp\RSPA05.tmp" with contents
[
/nologo /W3 /I "." /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "_LIB" /D "_MSC_VER" /D "HAVE_CONFIG_H" /D "FLAC__NO_DLL" /FR"ARMV4Rel/" /Fp"ARMV4Rel/libFLAC_static.pch" /YX /Fo"ARMV4Rel/" /O2 /MC /c 
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\bitbuffer.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\bitmath.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\cpu.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\crc.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\file_decoder.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\file_encoder.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\fixed.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\float.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\format.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\lpc.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\md5.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\memory.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\metadata_object.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\seekable_stream_decoder.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\seekable_stream_encoder.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\stream_decoder.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\stream_encoder.c"
"H:\BUILD\NITROGEN\FLAC\src\libFLAC\stream_encoder_framing.c"
]
Creating command line "clarm.exe @c:\tmp\RSPA05.tmp" 
Creating temporary file "c:\tmp\RSPA06.tmp" with contents
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
Creating command line "link.exe -lib @c:\tmp\RSPA06.tmp"
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
Creating temporary file "c:\tmp\RSPA07.tmp" with contents
[
/nologo /o"ARMV4Rel/libFLAC_static.bsc" 
.\ARMV4Rel\bitbuffer.sbr
.\ARMV4Rel\bitmath.sbr
.\ARMV4Rel\cpu.sbr
.\ARMV4Rel\crc.sbr
.\ARMV4Rel\file_decoder.sbr
.\ARMV4Rel\file_encoder.sbr
.\ARMV4Rel\fixed.sbr
.\ARMV4Rel\float.sbr
.\ARMV4Rel\format.sbr
.\ARMV4Rel\lpc.sbr
.\ARMV4Rel\md5.sbr
.\ARMV4Rel\memory.sbr
.\ARMV4Rel\metadata_object.sbr
.\ARMV4Rel\seekable_stream_decoder.sbr
.\ARMV4Rel\seekable_stream_encoder.sbr
.\ARMV4Rel\stream_decoder.sbr
.\ARMV4Rel\stream_encoder.sbr
.\ARMV4Rel\stream_encoder_framing.sbr]
Creating command line "bscmake.exe @c:\tmp\RSPA07.tmp"
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
libFLAC_static.lib - 0 error(s), 1 warning(s)
</pre>
</body>
</html>
