mkdir -p PyramidAndCubeColor.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o PyramidAndCubeColor.app/Contents/MacOS/PyramidAndCubeColor PyramidAndCubeColor.mm -framework Cocoa -framework quartzCore -framework OpenGL
