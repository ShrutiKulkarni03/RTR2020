cl.exe /c /EHsc GraphPaperWithCircleUsingLines.cpp

rc.exe MyIcon.rc

link.exe GraphPaperWithCircleUsingLines.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

GraphPaperWithCircleUsingLines