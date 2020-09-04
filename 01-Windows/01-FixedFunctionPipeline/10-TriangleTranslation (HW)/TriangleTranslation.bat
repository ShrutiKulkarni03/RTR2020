cl.exe /c /EHsc TriangleTranslation.cpp

rc.exe MyIcon.rc

link.exe TriangleTranslation.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

TriangleTranslation
