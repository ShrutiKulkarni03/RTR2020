mkdir -p BlueScreen.app/Contents/MacOS

clang -Wno-deprecated-declarations -o BlueScreen.app/Contents/MacOS/BlueScreen BlueScreen.m -framework Cocoa -framework quartzCore -framework OpenGL
