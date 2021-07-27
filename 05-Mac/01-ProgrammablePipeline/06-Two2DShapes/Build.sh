mkdir -p Two2DShapes.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o Two2DShapes.app/Contents/MacOS/Two2DShapes Two2DShapes.mm -framework Cocoa -framework quartzCore -framework OpenGL
