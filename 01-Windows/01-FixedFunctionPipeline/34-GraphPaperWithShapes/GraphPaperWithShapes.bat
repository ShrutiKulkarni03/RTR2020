cl.exe /c /EHsc GraphPaperWithShapes.cpp

rc.exe MyIcon.rc

link.exe GraphPaperWithShapes.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

GraphPaperWithShapes