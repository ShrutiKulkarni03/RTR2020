cl.exe /c /EHsc GraphPaperWithRectangle.cpp

rc.exe MyIcon.rc

link.exe GraphPaperWithRectangle.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

GraphPaperWithRectangle