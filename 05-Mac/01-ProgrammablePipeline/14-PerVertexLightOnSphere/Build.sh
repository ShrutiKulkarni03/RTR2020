mkdir -p PVLightOnSphere.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o PVLightOnSphere.app/Contents/MacOS/PVLightOnSphere PVLightOnSphere.mm -framework Cocoa -framework quartzCore -framework OpenGL
