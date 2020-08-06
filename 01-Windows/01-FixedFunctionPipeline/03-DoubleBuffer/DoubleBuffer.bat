cl.exe /c /EHsc DoubleBuffer.cpp

rc.exe DoubleBuffer.res 

link.exe DoubleBuffer.obj DoubleBuffer.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

DoubleBuffer
