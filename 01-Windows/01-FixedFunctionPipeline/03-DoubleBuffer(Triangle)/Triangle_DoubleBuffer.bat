
cl.exe /c /EHsc Triangle_DoubleBuffer.cpp

//Resource Compile
rc.exe Triangle_DoubleBuffer.res 

link.exe Triangle_DoubleBuffer.obj Triangle_DoubleBuffer.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Triangle_DoubleBuffer
