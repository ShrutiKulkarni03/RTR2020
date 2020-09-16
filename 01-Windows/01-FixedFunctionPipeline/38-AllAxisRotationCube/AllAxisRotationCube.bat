cl.exe /c /EHsc AllAxisRotationCube.cpp

rc.exe MyIcon.rc

link.exe AllAxisRotationCube.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

AllAxisRotationCube