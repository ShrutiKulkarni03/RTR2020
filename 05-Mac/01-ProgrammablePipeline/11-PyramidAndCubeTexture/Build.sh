mkdir -p PyramidAndCubeTexture.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o PyramidAndCubeTexture.app/Contents/MacOS/PyramidAndCubeTexture PyramidAndCubeTexture.mm -framework Cocoa -framework quartzCore -framework OpenGL
