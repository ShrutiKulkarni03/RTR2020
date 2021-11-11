cl.exe /c /EHsc /MD Interleaved.cpp

rc.exe Resources.rc

link.exe Interleaved.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

Interleaved
