cl.exe /c /EHsc BlueWindowWM_PAINT.cpp

rc.exe MyIcon.rc

link.exe BlueWindowWM_PAINT.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

BlueWindowWM_PAINT