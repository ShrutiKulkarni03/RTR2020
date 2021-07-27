mkdir -p PerspectiveBlueQuad.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o PerspectiveBlueQuad.app/Contents/MacOS/PerspectiveBlueQuad PerspectiveBlueQuad.mm -framework Cocoa -framework quartzCore -framework OpenGL
