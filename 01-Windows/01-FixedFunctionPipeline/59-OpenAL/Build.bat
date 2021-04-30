cl.exe /c /EHsc /I "C:\Program Files (x86)\OpenAL 1.1 SDK\include" DynamicIndia.cpp

rc.exe Resource.rc

link.exe DynamicIndia.obj Resource.res user32.lib gdi32.lib /LIBPATH:"C:\Program Files (x86)\OpenAL 1.1 SDK\libs\Win32" /SUBSYSTEM:WINDOWS 


DynamicIndia