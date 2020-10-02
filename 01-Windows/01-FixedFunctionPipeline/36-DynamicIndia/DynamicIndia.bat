cl.exe /c /EHsc DynamicIndia.cpp

rc.exe Resource.rc

link.exe DynamicIndia.obj Resource.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

DynamicIndia