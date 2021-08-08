cl.exe /c /EHsc Ortho.cpp

rc.exe Resources.rc

link.exe Ortho.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

Ortho
