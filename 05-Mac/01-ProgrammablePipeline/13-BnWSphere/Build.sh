mkdir -p BnWSphere.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o BnWSphere.app/Contents/MacOS/BnWSphere BnWSphere.mm -framework Cocoa -framework quartzCore -framework OpenGL
