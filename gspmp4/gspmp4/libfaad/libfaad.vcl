<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: libfaad - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "c:\tmp\RSP29.tmp" with contents
[
/nologo /W3 /Oxt /Ob2 /I "../include" /D "NDEBUG" /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "FPM_DEFAULT" /D "OPT_ACCURACY" /Fp"ARMV4Rel/libfaad.pch" /YX /Fo"ARMV4Rel/" /MC /c 
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\bits.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\cfft.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\common.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\decoder.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\drc.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\drm_dec.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\error.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\filtbank.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\hcr.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\huffman.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\ic_predict.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\is.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\lt_predict.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\mdct.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\mp4.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\ms.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\output.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\pns.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\ps_dec.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\ps_syntax.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\pulse.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\rvlc.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_dct.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_dec.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_e_nf.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_fbt.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_hfadj.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_hfgen.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_huff.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_qmf.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_syntax.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_tf_grid.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\ssr.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\ssr_fb.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\ssr_ipqf.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\syntax.c"
"H:\build\nitrogen\gspmp4\gspmp4\libfaad\tns.c"
]
Creating command line "clarm.exe @c:\tmp\RSP29.tmp" 
Creating temporary file "c:\tmp\RSP2A.tmp" with contents
[
/nologo /out:"..\lib\ARMV4Rel\libfaad.lib" 
.\ARMV4Rel\bits.obj
.\ARMV4Rel\cfft.obj
.\ARMV4Rel\common.obj
.\ARMV4Rel\decoder.obj
.\ARMV4Rel\drc.obj
.\ARMV4Rel\drm_dec.obj
.\ARMV4Rel\error.obj
.\ARMV4Rel\filtbank.obj
.\ARMV4Rel\hcr.obj
.\ARMV4Rel\huffman.obj
.\ARMV4Rel\ic_predict.obj
.\ARMV4Rel\is.obj
.\ARMV4Rel\lt_predict.obj
.\ARMV4Rel\mdct.obj
.\ARMV4Rel\mp4.obj
.\ARMV4Rel\ms.obj
.\ARMV4Rel\output.obj
.\ARMV4Rel\pns.obj
.\ARMV4Rel\ps_dec.obj
.\ARMV4Rel\ps_syntax.obj
.\ARMV4Rel\pulse.obj
.\ARMV4Rel\rvlc.obj
.\ARMV4Rel\sbr_dct.obj
.\ARMV4Rel\sbr_dec.obj
.\ARMV4Rel\sbr_e_nf.obj
.\ARMV4Rel\sbr_fbt.obj
.\ARMV4Rel\sbr_hfadj.obj
.\ARMV4Rel\sbr_hfgen.obj
.\ARMV4Rel\sbr_huff.obj
.\ARMV4Rel\sbr_qmf.obj
.\ARMV4Rel\sbr_syntax.obj
.\ARMV4Rel\sbr_tf_grid.obj
.\ARMV4Rel\specrec.obj
.\ARMV4Rel\ssr.obj
.\ARMV4Rel\ssr_fb.obj
.\ARMV4Rel\ssr_ipqf.obj
.\ARMV4Rel\syntax.obj
.\ARMV4Rel\tns.obj
]
Creating command line "link.exe -lib @c:\tmp\RSP2A.tmp"
<h3>Output Window</h3>
Compiling...
bits.c
cfft.c
h:\build\nitrogen\gspmp4\gspmp4\libfaad\cfft_tab.h(559) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\cfft_tab.h(1113) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\cfft_tab.h(1180) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\cfft_tab.h(1441) : error C2159: more than one storage class specified
common.c
H:\build\nitrogen\gspmp4\gspmp4\libfaad\common.c(496) : warning C4244: '=' : conversion from 'unsigned __int32 ' to '__int8 ', possible loss of data
decoder.c
H:\build\nitrogen\gspmp4\gspmp4\libfaad\decoder.c(991) : warning C4018: '<' : signed/unsigned mismatch
drc.c
drm_dec.c
error.c
filtbank.c
h:\build\nitrogen\gspmp4\gspmp4\libfaad\kbd_win.h(1074) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\kbd_win.h(2171) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sine_win.h(1074) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sine_win.h(2172) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sine_win.h(2298) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sine_win.h(2815) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sine_win.h(3300) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sine_win.h(3817) : error C2159: more than one storage class specified
hcr.c
huffman.c
ic_predict.c
is.c
lt_predict.c
H:\build\nitrogen\gspmp4\gspmp4\libfaad\lt_predict.c(70) : error C2159: more than one storage class specified
H:\build\nitrogen\gspmp4\gspmp4\libfaad\lt_predict.c(151) : warning C4244: 'return' : conversion from '__int32 ' to '__int16 ', possible loss of data
mdct.c
h:\build\nitrogen\gspmp4\gspmp4\libfaad\mdct_tab.h(630) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\mdct_tab.h(893) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\mdct_tab.h(1379) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\mdct_tab.h(1625) : error C2159: more than one storage class specified
mp4.c
ms.c
output.c
pns.c
H:\build\nitrogen\gspmp4\gspmp4\libfaad\pns.c(136) : warning C4244: '=' : conversion from '__int64 ' to '__int32 ', possible loss of data
ps_dec.c
ps_syntax.c
pulse.c
rvlc.c
sbr_dct.c
sbr_dec.c
sbr_e_nf.c
sbr_fbt.c
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_fbt.c(694) : warning C4244: '=' : conversion from '__int64 ' to '__int32 ', possible loss of data
sbr_hfadj.c
sbr_hfgen.c
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_hfgen.c(431) : warning C4244: '=' : conversion from '__int64 ' to '__int32 ', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_hfgen.c(433) : warning C4244: '=' : conversion from '__int64 ' to '__int32 ', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_hfgen.c(448) : warning C4244: '=' : conversion from '__int64 ' to '__int32 ', possible loss of data
h:\build\nitrogen\gspmp4\gspmp4\libfaad\sbr_hfgen.c(450) : warning C4244: '=' : conversion from '__int64 ' to '__int32 ', possible loss of data
sbr_huff.c
sbr_qmf.c
sbr_syntax.c
sbr_tf_grid.c
specrec.c
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(69) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(73) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(128) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(135) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(157) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(164) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(181) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(188) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(241) : error C2159: more than one storage class specified
h:\build\nitrogen\gspmp4\gspmp4\libfaad\specrec.c(257) : error C2159: more than one storage class specified
ssr.c
ssr_fb.c
ssr_ipqf.c
syntax.c
tns.c
Error executing clarm.exe.



<h3>Results</h3>
libfaad.lib - 27 error(s), 9 warning(s)
</pre>
</body>
</html>
