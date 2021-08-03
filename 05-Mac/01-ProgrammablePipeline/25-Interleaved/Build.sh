mkdir -p Interleaved.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o Interleaved.app/Contents/MacOS/Interleaved Interleaved.mm -framework Cocoa -framework quartzCore -framework OpenGL
