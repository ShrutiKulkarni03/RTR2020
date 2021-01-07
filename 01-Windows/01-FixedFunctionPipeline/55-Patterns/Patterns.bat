cl.exe /c /EHsc Patterns.cpp

rc.exe Resources.rc

link.exe Patterns.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Patterns