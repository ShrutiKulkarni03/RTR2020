mkdir -p PerspectiveMulticolorTriangle.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o PerspectiveMulticolorTriangle.app/Contents/MacOS/PerspectiveMulticolorTriangle PerspectiveMulticolorTriangle.mm -framework Cocoa -framework quartzCore -framework OpenGL
