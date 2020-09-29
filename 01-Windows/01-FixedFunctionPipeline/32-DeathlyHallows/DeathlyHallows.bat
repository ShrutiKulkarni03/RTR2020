cl.exe /c /EHsc DeathlyHallows.cpp

rc.exe MyIcon.rc

link.exe DeathlyHallows.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

DeathlyHallows