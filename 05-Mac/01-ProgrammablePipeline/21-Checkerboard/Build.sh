mkdir -p Checkerboard.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o Checkerboard.app/Contents/MacOS/Checkerboard Checkerboard.mm -framework Cocoa -framework quartzCore -framework OpenGL
