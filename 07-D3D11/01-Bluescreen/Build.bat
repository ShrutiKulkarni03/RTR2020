cl.exe /c /EHsc D3D11Bluescreen.cpp

rc.exe Resources.rc

link.exe D3D11Bluescreen.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

D3D11Bluescreen
