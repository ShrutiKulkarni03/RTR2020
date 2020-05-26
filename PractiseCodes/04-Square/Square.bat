REM Compile
cl.exe /c /EHsc /I C:\freeglut\include Square_with_Triangles.cpp

REM Link
link.exe Square_with_Triangles.obj /LIBPATH:C:\freeglut\lib /SUBSYSTEM:CONSOLE

REM Execute
Square_with_Triangles.exe