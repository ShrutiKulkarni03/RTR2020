cl.exe /c /EHsc Triangle_SingleBuffer.cpp

//Resource Compile
rc.exe Triangle_SingleBuffer.res 

link.exe Triangle_SingleBuffer.obj Triangle_SingleBuffer.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Triangle_SingleBuffer
