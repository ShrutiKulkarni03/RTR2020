cl.exe /c /EHsc MonkeyHead.cpp

rc.exe Resources.rc

link.exe MonkeyHead.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

MonkeyHead
