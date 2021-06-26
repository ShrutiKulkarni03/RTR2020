cl.exe /c /EHsc /I C:\glew-2.1.0\include TesselationShader.cpp

rc.exe Resources.rc

link.exe TesselationShader.obj Resources.res user32.lib gdi32.lib /LIBPATH:"C:\glew-2.1.0\lib\Release\Win32" /SUBSYSTEM:WINDOWS 

TesselationShader

