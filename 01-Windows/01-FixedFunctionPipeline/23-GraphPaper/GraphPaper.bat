cl.exe /c /EHsc GraphPaper.cpp

rc.exe MyIcon.rc

link.exe GraphPaper.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

GraphPaper