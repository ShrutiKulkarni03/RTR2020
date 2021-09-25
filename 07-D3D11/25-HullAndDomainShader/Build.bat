cl.exe /c /EHsc HullAndDomainShader.cpp

rc.exe Resources.rc

link.exe HullAndDomainShader.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

HullAndDomainShader
