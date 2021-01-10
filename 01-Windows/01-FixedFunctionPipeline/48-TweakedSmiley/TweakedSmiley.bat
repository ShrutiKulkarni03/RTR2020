cl.exe /c /EHsc TweakedSmiley.cpp

rc.exe MyIcon.rc 

link.exe TweakedSmiley.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

TweakedSmiley
