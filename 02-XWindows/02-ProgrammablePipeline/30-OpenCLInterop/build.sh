g++ -c -o OpenCLInterop.obj OpenCLInterop.cpp
g++ -o OpenCLInterop OpenCLInterop.obj -lX11 -lGL -lGLEW -lOpenCL
./OpenCLInterop

