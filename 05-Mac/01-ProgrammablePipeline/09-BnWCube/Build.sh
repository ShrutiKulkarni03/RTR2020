mkdir -p BnWCube.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o BnWCube.app/Contents/MacOS/BnWCube BnWCube.mm -framework Cocoa -framework quartzCore -framework OpenGL
