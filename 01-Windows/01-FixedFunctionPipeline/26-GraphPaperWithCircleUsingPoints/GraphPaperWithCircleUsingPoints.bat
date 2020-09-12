cl.exe /c /EHsc GraphPaperWithCircleUsingPoints.cpp

rc.exe MyIcon.rc

link.exe GraphPaperWithCircleUsingPoints.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

GraphPaperWithCircleUsingPoints