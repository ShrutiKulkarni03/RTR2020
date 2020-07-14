cl.exe /c /EHsc BlueWindow.cpp

rc.exe MyIcon.rc

link.exe BlueWindow.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

BlueWindow
