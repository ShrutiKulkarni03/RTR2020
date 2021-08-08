mkdir -p PFLightOnSphere.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o PFLightOnSphere.app/Contents/MacOS/PFLightOnSphere PFLightOnSphere.mm -framework Cocoa -framework quartzCore -framework OpenGL
