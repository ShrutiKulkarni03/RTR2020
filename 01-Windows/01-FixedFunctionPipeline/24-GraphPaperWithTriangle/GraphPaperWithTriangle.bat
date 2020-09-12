cl.exe /c /EHsc GraphPaperWithTriangle.cpp

rc.exe MyIcon.rc

link.exe GraphPaperWithTriangle.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

GraphPaperWithTriangle