cl.exe /c /EHsc OpenGLTemplate.cpp

rc.exe MyIcon.rc

link.exe OpenGLTemplate.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

OpenGLTemplate