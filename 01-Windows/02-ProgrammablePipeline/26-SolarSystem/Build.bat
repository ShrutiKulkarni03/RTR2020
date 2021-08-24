cl.exe /c /EHsc /I C:\glew-2.1.0\include SolarSystem.cpp

rc.exe Resources.rc

link.exe SolarSystem.obj Resources.res user32.lib gdi32.lib /LIBPATH:"C:\glew-2.1.0\lib\Release\Win32" /SUBSYSTEM:WINDOWS 

SolarSystem

