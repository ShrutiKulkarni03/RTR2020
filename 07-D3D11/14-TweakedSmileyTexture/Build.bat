cl.exe /c /EHsc /MD TweakedSmiley.cpp

rc.exe Resources.rc

link.exe TweakedSmiley.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

TweakedSmiley
