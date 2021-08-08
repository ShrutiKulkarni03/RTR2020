mkdir -p PVPFToggle.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o PVPFToggle.app/Contents/MacOS/PVPFToggle PVPFToggle.mm -framework Cocoa -framework quartzCore -framework OpenGL
