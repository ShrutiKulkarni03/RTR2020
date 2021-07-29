mkdir -p Two2DShapesAnimation.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o Two2DShapesAnimation.app/Contents/MacOS/Two2DShapesAnimation Two2DShapesAnimation.mm -framework Cocoa -framework quartzCore -framework OpenGL
