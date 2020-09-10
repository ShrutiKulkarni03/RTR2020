cl.exe /c /EHsc ModelAndCameraTransformation.cpp

rc.exe MyIcon.res

link.exe ModelAndCameraTransformation.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

ModelAndCameraTransformation