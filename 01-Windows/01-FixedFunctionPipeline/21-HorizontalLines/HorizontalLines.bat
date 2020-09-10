cl.exe /c /EHsc HorizontalLines.cpp

rc.exe MyIcon.res

link.exe HorizontalLines.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

HorizontalLines