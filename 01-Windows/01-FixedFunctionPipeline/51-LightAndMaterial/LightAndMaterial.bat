cl.exe /c /EHsc LightAndMaterial.cpp

rc.exe MyIcon.rc

link.exe LightAndMaterial.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

LightAndMaterial
