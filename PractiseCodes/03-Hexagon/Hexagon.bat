REM Compile
cl.exe /c /EHsc /I C:\freeglut\include Hexagon.cpp

REM Link
link.exe Hexagon.obj /LIBPATH:C:\freeglut\lib /SUBSYSTEM:CONSOLE

REM Execute
Hexagon.exe