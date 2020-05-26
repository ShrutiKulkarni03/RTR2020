REM Compile
cl.exe /c /EHsc /I C:\freeglut\include ColorWheel.cpp

REM Link
link.exe ColorWheel.obj /LIBPATH:C:\freeglut\lib /SUBSYSTEM:CONSOLE

REM Execute
ColorWheel.exe
