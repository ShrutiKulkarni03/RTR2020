cl.exe /c /EHsc /MDd /I "C:\glew-2.1.0\include" /I "C:\SOIL\inc\SOIL" Godrays.cpp

rc.exe Resources.rc

link.exe Godrays.obj Resources.res user32.lib gdi32.lib /LIBPATH:"C:\glew-2.1.0\lib\Release\x64" /LIBPATH:"C:\SOIL\Debug" /SUBSYSTEM:WINDOWS 
Godrays


