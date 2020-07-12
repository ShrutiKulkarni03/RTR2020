cl.exe /c /EHsc GUIFileIO.cpp

link.exe GUIFileIO.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

GUIFileIO