mkdir -p TwoLightsOnPyramid.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o TwoLightsOnPyramid.app/Contents/MacOS/TwoLightsOnPyramid TwoLightsOnPyramid.mm -framework Cocoa -framework quartzCore -framework OpenGL
