cl.exe /c /EHsc /I "C:\glew-2.1.0\include" /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.4\include" OpenCLInterop.cpp

rc.exe Resources.rc

link.exe OpenCLInterop.obj Resources.res user32.lib gdi32.lib /LIBPATH:"C:\glew-2.1.0\lib\Release\x64" /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.4\lib\x64" /MACHINE:X64 /SUBSYSTEM:WINDOWS 

OpenCLInterop
