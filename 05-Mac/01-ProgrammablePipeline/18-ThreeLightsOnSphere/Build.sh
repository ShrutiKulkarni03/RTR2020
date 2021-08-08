mkdir -p ThreeLightsOnSphere.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o ThreeLightsOnSphere.app/Contents/MacOS/ThreeLightsOnSphere ThreeLightsOnSphere.mm -framework Cocoa -framework quartzCore -framework OpenGL
