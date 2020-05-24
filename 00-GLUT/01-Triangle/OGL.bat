REM compile 
cl.exe /c /EHsc /I C:\freeglut\include OGL.cpp

REM link
lnk.exe OGL.obj /LIBPATH:C:\freeglut\lib /SUBSYSTEM:CONSOLE

REM execute
OGL