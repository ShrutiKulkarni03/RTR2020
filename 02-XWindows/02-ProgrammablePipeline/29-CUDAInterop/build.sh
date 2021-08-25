nvcc -c -arch compute_35 -o SineWave.cu.obj SineWave.cu
g++ -c -o CUDAInterop.obj CUDAInterop.cpp
g++ -o CUDAInterop SineWave.cu.obj CUDAInterop.obj -lX11 -lGL -lGLEW -lcudart -lc -lm -lstdc++
./CUDAInterop

