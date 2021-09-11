cl.exe /c /EHsc DiffuseLightOnCube.cpp

rc.exe Resources.rc

link.exe DiffuseLightOnCube.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

DiffuseLightOnCube
