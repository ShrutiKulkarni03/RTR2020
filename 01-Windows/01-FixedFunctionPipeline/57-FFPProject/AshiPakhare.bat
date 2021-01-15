cl.exe /c /EHsc AshiPakhare.cpp

rc.exe Resources.rc

link.exe AshiPakhare.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

AshiPakhare